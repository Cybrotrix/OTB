/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "otbONERAImageIO.h"
#include "itkByteSwapper.h"
#include <itksys/SystemTools.hxx>
#include <string.h>  // for strncpy

#include "itkExceptionObject.h"
#include "otbMacro.h"
#include "itkMacro.h"
#include <stdio.h>

#include "otbSystem.h"

#define ONERA_MAGIC_NUMBER     33554433
#define ONERA_HEADER_LENGTH    4
#define ONERA_FILE_ID_OFFSET   0

namespace otb
{

ONERAImageIO::ONERAImageIO()
{
  // By default set number of dimensions to two.
  this->SetNumberOfDimensions(2);
  m_PixelType = COMPLEX;
  m_ComponentType = FLOAT;
  m_NbOctetPixel = 4;

  // Set default spacing to one
  m_Spacing[0] = 1.0;
  m_Spacing[1] = 1.0;
  // Set default origin to zero
  m_Origin[0] = 0.0;
  m_Origin[1] = 0.0;

  m_FlagWriteImageInformation = true;


}

ONERAImageIO::~ONERAImageIO()
{
  if( m_Datafile.is_open() )
    {
    m_Datafile.close();
    }
  if( m_Headerfile.is_open() )
    {
    m_Headerfile.close();
    }
}




static std::string
GetHeaderFileName( const std::string & filename )
{
  std::string ImageFileName = System::GetRootName(filename);
  std::string fileExt = System::GetExtension(filename);

  if(fileExt.compare("ent"))
    {
    ImageFileName += ".ent";
    
    return( ImageFileName );
    }
  else
    {
    return("");
    }  
}

//Returns the base image filename.
static std::string GetImageFileName( const std::string& filename )
{
  std::string fileExt = System::GetExtension(filename);
  std::string ImageFileName = System::GetRootName(filename);

  if(fileExt.compare("dat") )
    {
    ImageFileName += ".dat";
    return( ImageFileName );
    }
  else
    {
    //uiig::Reporter* reporter = uiig::Reporter::getReporter();
    //std::string temp="Error, Can not determine compressed file image name. ";
    //temp+=filename;
    //reporter->setMessage( temp );
    return ("");
    }
}

bool ONERAImageIO::CanReadFile( const char* FileNameToRead )
{

  std::string filename(FileNameToRead);
  std::string ext;
  if( m_Datafile.is_open() )
    {
    m_Datafile.close();
    }
  if( m_Headerfile.is_open() )
    {
    m_Headerfile.close();
    }

  const std::string HeaderFileName = System::GetRootName(filename)+".ent";
  const std::string DataFileName = System::GetRootName(filename)+".dat";

  m_Headerfile.open( HeaderFileName.c_str(),  std::ios::in );
  if( m_Headerfile.fail() )
    {
    otbMsgDevMacro(<<"ONERAImageIO::CanReadFile() failed header open ! " );
    return false;
    }
  m_Datafile.open( DataFileName.c_str(),  std::ios::in );
  if( m_Datafile.fail() )
    {
    otbMsgDevMacro(<<"ONERAImageIO::CanReadFile() failed data open ! " );
    return false;
    }

  this->m_ByteOrder = LittleEndian;

  // Check magic_number
  int magicNumber; 
  m_Datafile.seekg(0, std::ios::beg );
  m_Datafile.read((char*)(&magicNumber),4);

  itk::ByteSwapper< int>::SwapFromSystemToLittleEndian(&magicNumber);

  m_Headerfile.close();
  m_Datafile.close();

  if( magicNumber == ONERA_MAGIC_NUMBER )
  {
        return true;
  }
  else
  {
        return false;
  }

}




// Used to print information about this object
void ONERAImageIO::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

// Read a 3D image (or event more bands)... not implemented yet
void ONERAImageIO::ReadVolume(void*)
{
}


// Read image 
void ONERAImageIO::Read(void* buffer)
{
   unsigned int       dim;
   const unsigned int dimensions = this->GetNumberOfDimensions();
   unsigned long        numberOfPixels = 1;
   for(dim=0; dim< dimensions; dim++ )
      {
      numberOfPixels *= m_Dimensions[ dim ];
      }

   unsigned long step = this->GetNumberOfComponents();
   unsigned char * p = static_cast<unsigned char *>(buffer);
   
   int lNbLignes   = this->GetIORegion().GetSize()[1];
   int lNbColonnes = this->GetIORegion().GetSize()[0];
   int lPremiereLigne   = this->GetIORegion().GetIndex()[1] ; // [1... ]
   int lPremiereColonne = this->GetIORegion().GetIndex()[0] ; // [1... ]

otbMsgDevMacro( <<" ONERAImageIO::Read()  ");
otbMsgDevMacro( <<" Dimensions de l'image  : "<<m_Dimensions[0]<<","<<m_Dimensions[1]);
otbMsgDevMacro( <<" Region lue (IORegion)  : "<<this->GetIORegion());
otbMsgDevMacro( <<" Nb Of Components  : "<<this->GetNumberOfComponents());

  //read header information file:
  if ( ! this->OpenOneraDataFileForReading(m_FileName.c_str()) )
    {
    itkExceptionMacro(<< "Cannot read requested file");
    }

  unsigned long numberOfBytesPerLines = 2 * m_width * m_NbOctetPixel;
  unsigned long headerLength = ONERA_HEADER_LENGTH + numberOfBytesPerLines;
  unsigned long offset;
  unsigned long numberOfBytesToBeRead = 2 * m_NbOctetPixel *lNbColonnes;
  unsigned long numberOfBytesRead;        

otbMsgDevMacro( <<" ONERAImageIO::Read()  ");
otbMsgDevMacro( <<" Dimensions de l'image  : "<<m_Dimensions[0]<<","<<m_Dimensions[1]);
otbMsgDevMacro( <<" Region lue (IORegion)  : "<<this->GetIORegion());
otbMsgDevMacro( <<" Nb Of Components  : "<<this->GetNumberOfComponents());

  char* value = new char[numberOfBytesToBeRead];
  unsigned long cpt = 0;
 
  for(int LineNo = lPremiereLigne;LineNo <lPremiereLigne + lNbLignes; LineNo++ )
    {
	offset  =  headerLength + numberOfBytesPerLines * LineNo;
	offset +=  m_NbOctetPixel * lPremiereColonne;
  	m_Datafile.seekg(offset, std::ios::beg);
        m_Datafile.read( static_cast<char *>( value ), numberOfBytesToBeRead );
    	numberOfBytesRead = m_Datafile.gcount();
        if( numberOfBytesRead != numberOfBytesToBeRead )
	  if( ( numberOfBytesRead != numberOfBytesToBeRead )  || m_Datafile.fail() )
             {
      		itkExceptionMacro(<<"ONERAImageIO::Read() Can Read the specified Region"); // read failed
      	     }

	for( unsigned long  i=0 ; i < numberOfBytesRead ; i = i+  2 * m_NbOctetPixel )
           {
            memcpy((void*)(&(p[cpt])),(const void*)(&(value[i])),(size_t)( 2* m_NbOctetPixel));
            cpt += 2*m_NbOctetPixel;
           }
    }

  //byte swapping depending on pixel type:
  if(this->GetComponentType() == FLOAT)
    {
    itk::ByteSwapper<float>::SwapRangeFromSystemToLittleEndian(
        reinterpret_cast<float *>(buffer),cpt);
    }
}


bool ONERAImageIO::OpenOneraDataFileForReading(const char* filename)                                       
{
  // Make sure that we have a file to 
  if ( filename == "" )
    {
    itkExceptionMacro(<<"A FileName must be specified.");
    return false;
    }

  // Close file from any previous image
  if ( m_Datafile.is_open() )
    {
    m_Datafile.close();
    }
  const std::string DataFileName = System::GetRootName(filename)+".dat";
  
  // Open the new file for reading
  m_Datafile.open( DataFileName.c_str(),  std::ios::in );
  if( m_Datafile.fail() )
    {
    otbMsgDebugMacro(<<"ONERAImageIO::CanReadFile() failed data open ! " );
    return false;
    }
  return true;
}

bool ONERAImageIO::OpenOneraHeaderFileForReading(const char* filename)                                       
{
  // Make sure that we have a file to 
  if ( filename == "" )
    {
    itkExceptionMacro(<<"A FileName must be specified.");
    return false;
    }

  // Close file from any previous image
  if ( m_Headerfile.is_open() )
    {
    m_Headerfile.close();
    }
  const std::string HeaderFileName = System::GetRootName(filename)+".ent";
  
  // Open the new file for reading
  // Actually open the file
  m_Headerfile.open( HeaderFileName.c_str(),  std::ios::in );
  if( m_Headerfile.fail() )
    {
    otbMsgDebugMacro(<<"ONERAImageIO::CanReadFile() failed header open ! " );
    return false;
    }
  return true;
}

void ONERAImageIO::ReadImageInformation()
{
  this->InternalReadImageInformation();
}




void ONERAImageIO::InternalReadImageInformation()
{


  if ( ! this->OpenOneraDataFileForReading(m_FileName.c_str()) )
    {
    itkExceptionMacro(<< "Cannot read requested file");
    }

  if ( ! this->OpenOneraDataFileForReading(m_FileName.c_str()) )
    {
    itkExceptionMacro(<< "Cannot read requested file");
    }

  // check "Format_valeurs_look"  
  char* sHeader = new char[1024];
  // skip 2 lines 	
  m_Headerfile.getline(sHeader,1024);
  m_Headerfile.getline(sHeader,1024);
  m_Headerfile.getline(sHeader,1024);
  std::string sPixelType(sHeader);

  if ( sPixelType.compare("cmplx_real_4") )
    {
      m_NbBands = 1;
      this->SetFileTypeToBinary();
      this->SetNumberOfComponents(2);
      this->SetPixelType(COMPLEX);
      m_ComponentType = FLOAT;
      m_NbOctetPixel = 4;
    }
  else
    {
    itkExceptionMacro(<< "data format not supported by OTB (only 'complex_real_4' is available)");  
    }  

  // Check magic_number
  int magicNumber; 
  m_Datafile.seekg(0, std::ios::beg );
  m_Datafile.read((char*)(&magicNumber),4);
  itk::ByteSwapper< int>::SwapFromSystemToLittleEndian(&magicNumber);

  // Find info.. : Number of Row , Nb of Columns
  short NbCol;

  m_Datafile.seekg(ONERA_HEADER_LENGTH + 2, std::ios::beg );
  m_Datafile.read((char*)(&NbCol),2);  
  itk::ByteSwapper<short>::SwapFromSystemToLittleEndian(&NbCol);
    
  m_Datafile.seekg(0, std::ios::end);
  long gcountHead = static_cast<long>(ONERA_HEADER_LENGTH + 2*4*NbCol);
  long gcount     = static_cast<long>(m_Datafile.tellg());

  // Defining the image size:
  m_width = static_cast<int> ( NbCol );
  m_height = static_cast<int> ( (gcount-gcountHead) / (4 * 2 * NbCol) );

  if( (m_width==0) || (m_height==0))
      {
      itkExceptionMacro(<<"Unknown image dimension");
      }
    else
      {
      // Set image dimensions into IO
      m_Dimensions[0] = m_width;
      m_Dimensions[1] = m_height;
      }

  this->SetNumberOfDimensions(2);  

otbMsgDebugMacro( <<"Driver to read: ONERA");
otbMsgDebugMacro( <<"         Read  file         : "<< m_FileName);
otbMsgDebugMacro( <<"         Size               : "<<m_Dimensions[0]<<","<<m_Dimensions[1]);
otbMsgDebugMacro( <<"         ComponentType      : "<<this->GetComponentType() );
otbMsgDebugMacro( <<"         NumberOfComponents : "<<this->GetNumberOfComponents());
otbMsgDebugMacro( <<"         NbOctetPixel       : "<<m_NbOctetPixel);

}


bool ONERAImageIO::OpenOneraDataFileForWriting(const char* filename)                                       
{
  // Make sure that we have a file to 
  if ( filename == "" )
    {
    itkExceptionMacro(<<"A FileName must be specified.");
    return false;
    }

  // Close file from any previous image
  if ( m_Datafile.is_open() )
    {
    m_Datafile.close();
    }
  const std::string DataFileName = System::GetRootName(filename)+".dat";
  
  // Open the new file for reading

  // Actually open the file
  m_Datafile.open( DataFileName.c_str(),  std::ios::out | std::ios::trunc | std::ios::binary);
  if( m_Datafile.fail() )
    {
    otbMsgDebugMacro(<<"ONERAImageIO::OpenOneraDataFileForWriting() failed data open ! " );
    return false;
    }
  return true;
}

bool ONERAImageIO::OpenOneraHeaderFileForWriting(const char* filename)                                       
{
  // Make sure that we have a file to 
  if ( filename == "" )
    {
    itkExceptionMacro(<<"A FileName must be specified.");
    return false;
    }

  // Close file from any previous image
  if ( m_Headerfile.is_open() )
    {
    m_Headerfile.close();
    }
  const std::string HeaderFileName = System::GetRootName(filename)+".ent";
  
  // Open the new file for reading
  // Actually open the file
  m_Headerfile.open( HeaderFileName.c_str(),  std::ios::out | std::ios::trunc | std::ios::binary );
  if( m_Headerfile.fail() )
    {
    otbMsgDebugMacro(<<"ONERAImageIO::OpenOneraHeaderFileForWriting() failed header open ! " );
    return false;
    }
  return true;
}



bool ONERAImageIO::CanWriteFile( const char* FileNameToWrite )
{
  std::string filename(FileNameToWrite);

  const std::string HeaderFileName = System::GetRootName(filename)+".ent";
  const std::string DataFileName = System::GetRootName(filename)+".dat";

  if( filename == HeaderFileName )
  {
        return true;
  }
  else
  {
        return false;
  }
}

void ONERAImageIO::Write(const void* buffer)
{
   
  if( m_FlagWriteImageInformation == true )
  {
    	this->WriteImageInformation();
  	m_FlagWriteImageInformation = false;
  }

  unsigned long step = this->GetNumberOfComponents();

  const unsigned long numberOfBytes      = this->GetImageSizeInBytes();
  const unsigned long numberOfComponents = this->GetImageSizeInComponents();

  int lNbLignes   = this->GetIORegion().GetSize()[1];
  int lNbColonnes = this->GetIORegion().GetSize()[0];
  int lPremiereLigne   = this->GetIORegion().GetIndex()[1] ; // [1... ]
  int lPremiereColonne = this->GetIORegion().GetIndex()[0] ; // [1... ]

otbMsgDevMacro( <<" ONERAImageIO::Write()  ");
otbMsgDevMacro( <<" Dimensions de l'image  : "<<m_Dimensions[0]<<","<<m_Dimensions[1]);
otbMsgDevMacro( <<" Region lue (IORegion)  : "<<this->GetIORegion());
otbMsgDevMacro( <<" Nb Of Components  : "<<this->GetNumberOfComponents());

	// Cas particuliers : on controle que si la r�gion � �crire est de la m�me dimension que l'image enti�re,
	// on commence l'offset � 0 (lorsque que l'on est pas en "Streaming")
	if( (lNbLignes == m_Dimensions[1]) && (lNbColonnes == m_Dimensions[0]))
	{
                otbMsgDevMacro(<<"Force l'offset de l'IORegion � 0");
		lPremiereLigne = 0;
		lPremiereColonne = 0;
	}

  unsigned long numberOfBytesPerLines = step * lNbColonnes * m_NbOctetPixel;
  unsigned long headerLength = ONERA_HEADER_LENGTH + numberOfBytesPerLines;
  unsigned long offset;
  unsigned long numberOfBytesRegion = step * m_NbOctetPixel *lNbColonnes *lNbLignes;

  char *tempmemory = new char[numberOfBytesRegion];
  memcpy(tempmemory,buffer,numberOfBytesRegion);

  if(this->GetComponentType() == FLOAT)
    {
    itk::ByteSwapper<float>::SwapRangeFromSystemToLittleEndian(
        reinterpret_cast<float *>(tempmemory), numberOfComponents );
    }
 
  for(int LineNo = lPremiereLigne;LineNo <lPremiereLigne + lNbLignes; LineNo++ )
    {
	char* value = tempmemory + numberOfBytesPerLines * (LineNo - lPremiereLigne);

	offset  =  headerLength + numberOfBytesPerLines * LineNo;
	offset +=  m_NbOctetPixel * lPremiereColonne;
  	m_Datafile.seekg(offset, std::ios::beg);
	m_Datafile.write( static_cast<char *>( value ), numberOfBytesPerLines );
    }

  delete [] tempmemory;
}

void ONERAImageIO::WriteImageInformation()
{

  if ( ! this->OpenOneraHeaderFileForWriting(m_FileName.c_str()) )
    {
    itkExceptionMacro(<< "Cannot read requested file");
    }

  if ( ! this->OpenOneraDataFileForWriting(m_FileName.c_str()) )
    {
    itkExceptionMacro(<< "Cannot read requested file");
    }

  /*-------- This part deals with writing header information ------ */
  const std::string DataFileName = System::GetRootName( m_FileName.c_str() )+".dat";

  m_Headerfile << "#                    [fichier en-tete produit par les routines de otb (Orfeo ToolBox) ]" << std::endl;
  m_Headerfile << "# Nom du look :"<< std::endl;
  m_Headerfile << "Look.dat= \t" << DataFileName.c_str() <<  std::endl;
  m_Headerfile << std::endl;
  m_Headerfile << "# Structure du fichier et codage des pixels :"<< std::endl;
  m_Headerfile << "# 4 octets precedent la premiere ligne : ils correspondent a un nombre magique [I4= 33554433] "<< std::endl;
  m_Headerfile << "# [dans ordre LSBfirst = big-endian]" << std::endl;

  std::string sPixelType("cmplx_real_4");
  if( (m_PixelType == COMPLEX) && (m_ComponentType == FLOAT) )
    {
    sPixelType = "cmplx_real_4";
    }
  else
    {
    itkExceptionMacro(<< "data format not supported by OTB (only 'complex_real_4' is available)");  
    }  
       
  m_Headerfile << "Format_valeurs_look=    \t"<< sPixelType << std::endl;
  m_Headerfile << "Nb_case_par_ligne_look= \t"<< m_Dimensions[0] <<std::endl;  
  m_Headerfile << "Nb_ligne_look=          \t"<< m_Dimensions[1] <<" + 1 ligne en-tete en binaire (entiers 16 bit) " << std::endl;

  // write magic_number
  int magicNumber = ONERA_MAGIC_NUMBER; 
  short NbCol = static_cast<short>(m_Dimensions[0]);
  short NbRow = static_cast<short>(m_Dimensions[1]);
  int ByteSizeCol = NbCol*4*2;

  itk::ByteSwapper< int>::SwapFromSystemToLittleEndian(&magicNumber);
  m_Datafile.seekg(0, std::ios::beg );
  m_Datafile.write((char*)(&magicNumber),4);
  
  char * tab = new char[ByteSizeCol];
  for( int i = 0 ; i < (NbRow + 1) ; i++)
  {
  	m_Datafile.write((char*)(tab),ByteSizeCol);
  }
  delete [] tab;

  // write number of columns
  itk::ByteSwapper<short>::SwapFromSystemToLittleEndian(&NbCol);

  m_Datafile.seekg(ONERA_HEADER_LENGTH+2, std::ios::beg );
  m_Datafile.write((char*)(&NbCol),2);
  
  
otbMsgDebugMacro( <<"Driver to write: ONERA");
otbMsgDebugMacro( <<"         Write file         : "<< m_FileName);
otbMsgDebugMacro( <<"         Size               : "<<m_Dimensions[0]<<","<<m_Dimensions[1]);
otbMsgDebugMacro( <<"         ComponentType      : "<<this->GetComponentType() );
otbMsgDebugMacro( <<"         NumberOfComponents : "<<this->GetNumberOfComponents());
otbMsgDebugMacro( <<"         NbOctetPixel       : "<<m_NbOctetPixel);



}

  
} // end namespace otb

