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
#include "otbWrapperOutputImageParameter.h"
#include "itkCastImageFilter.h"
#include "itkVectorCastImageFilter.h"

namespace otb
{
namespace Wrapper
{

OutputImageParameter::OutputImageParameter()
  : m_PixelType(ImagePixelType_float), m_RAMValue(0)
{
  this->SetName("Output Image");
  this->SetKey("out");
}


OutputImageParameter::~OutputImageParameter()
{
}

void OutputImageParameter::InitializeWriters()
{
  m_Int8Writer = Int8WriterType::New();
  m_UInt8Writer = UInt8WriterType::New();
  m_Int16Writer = Int16WriterType::New();
  m_UInt16Writer = UInt16WriterType::New();
  m_Int32Writer = Int32WriterType::New();
  m_UInt32Writer = UInt32WriterType::New();
  m_FloatWriter = FloatWriterType::New();
  m_DoubleWriter = DoubleWriterType::New();

  m_VectorInt8Writer = VectorInt8WriterType::New();
  m_VectorUInt8Writer = VectorUInt8WriterType::New();
  m_VectorInt16Writer = VectorInt16WriterType::New();
  m_VectorUInt16Writer = VectorUInt16WriterType::New();
  m_VectorInt32Writer = VectorInt32WriterType::New();
  m_VectorUInt32Writer = VectorUInt32WriterType::New();
  m_VectorFloatWriter = VectorFloatWriterType::New();
  m_VectorDoubleWriter = VectorDoubleWriterType::New();

  m_RGBUInt8Writer = RGBUInt8WriterType::New();
  m_RGBAUInt8Writer = RGBAUInt8WriterType::New();
}


#define otbCastAndWriteImageMacro(InputImageType, OutputImageType, writer)        \
  {                                                                               \
    typedef itk::CastImageFilter<InputImageType, OutputImageType> CastFilterType; \
    typename CastFilterType::Pointer caster = CastFilterType::New();              \
    caster->SetInput( dynamic_cast<InputImageType*>(m_Image.GetPointer()) );      \
    caster->InPlaceOn();                                                          \
    writer->SetFileName( this->GetFileName() );                                   \
    writer->SetInput(caster->GetOutput());                                        \
    writer->WriteGeomFileOn();                                                    \
    writer->SetAutomaticAdaptativeStreaming(m_RAMValue);                          \
    writer->Update();                                                             \
  }


template <class TInputImageType>
void
OutputImageParameter::SwitchImageWrite()
{
  switch(m_PixelType )
    {
    case ImagePixelType_int8:
    {
    otbCastAndWriteImageMacro(TInputImageType, Int8ImageType, m_Int8Writer);
    break;
    }
    case ImagePixelType_uint8:
    {
    otbCastAndWriteImageMacro(TInputImageType, UInt8ImageType, m_UInt8Writer);
    break;
    }
    case ImagePixelType_int16:
    {
    otbCastAndWriteImageMacro(TInputImageType, Int16ImageType, m_Int16Writer);
    break;
    }
    case ImagePixelType_uint16:
    {
    otbCastAndWriteImageMacro(TInputImageType, UInt16ImageType, m_UInt16Writer);
    break;
    }
    case ImagePixelType_int32:
    {
    otbCastAndWriteImageMacro(TInputImageType, Int32ImageType, m_Int32Writer);
    break;
    }
    case ImagePixelType_uint32:
    {
    otbCastAndWriteImageMacro(TInputImageType, UInt32ImageType, m_UInt32Writer);
    break;
    }
    case ImagePixelType_float:
    {
    otbCastAndWriteImageMacro(TInputImageType, FloatImageType, m_FloatWriter);
    break;
    }
    case ImagePixelType_double:
    {
    otbCastAndWriteImageMacro(TInputImageType, DoubleImageType, m_DoubleWriter);
    break;
    }
    }
}


template <class TInputVectorImageType>
void
OutputImageParameter::SwitchVectorImageWrite()
  {
  switch(m_PixelType )
    {
    case ImagePixelType_int8:
    {
    otbCastAndWriteImageMacro(TInputVectorImageType, Int8VectorImageType, m_VectorInt8Writer);
    break;
    }
    case ImagePixelType_uint8:
    {
    otbCastAndWriteImageMacro(TInputVectorImageType, UInt8VectorImageType, m_VectorUInt8Writer);
    break;
    }
    case ImagePixelType_int16:
    {
    otbCastAndWriteImageMacro(TInputVectorImageType, Int16VectorImageType, m_VectorInt16Writer);
    break;
    }
    case ImagePixelType_uint16:
    {
    otbCastAndWriteImageMacro(TInputVectorImageType, UInt16VectorImageType, m_VectorUInt16Writer);
    break;
    }
    case ImagePixelType_int32:
    {
    otbCastAndWriteImageMacro(TInputVectorImageType, Int32VectorImageType, m_VectorInt32Writer);
    break;
    }
    case ImagePixelType_uint32:
    {
    otbCastAndWriteImageMacro(TInputVectorImageType, UInt32VectorImageType, m_VectorUInt32Writer);
    break;
    }
    case ImagePixelType_float:
    {
    otbCastAndWriteImageMacro(TInputVectorImageType, FloatVectorImageType, m_VectorFloatWriter);
    break;
    }
    case ImagePixelType_double:
    {
    otbCastAndWriteImageMacro(TInputVectorImageType, DoubleVectorImageType, m_VectorDoubleWriter);
    break;
    }
    }
  }


template <class TInputRGBAImageType>
void
OutputImageParameter::SwitchRGBAImageWrite()
  {
  if( m_PixelType == ImagePixelType_uint8 )
    {
    otbCastAndWriteImageMacro(TInputRGBAImageType, UInt8RGBAImageType, m_RGBAUInt8Writer);
    }
   else
     itkExceptionMacro("Unknown PixelType for RGBA Image.");
  }

template <class TInputRGBImageType>
void
OutputImageParameter::SwitchRGBImageWrite()
  {
   if( m_PixelType == ImagePixelType_uint8 )
    {
    otbCastAndWriteImageMacro(TInputRGBImageType, UInt8RGBImageType, m_RGBUInt8Writer);
    }
   else
     itkExceptionMacro("Unknown PixelType for RGB Image.");
  }

void
OutputImageParameter::Write()
{
  m_Image->UpdateOutputInformation();

  if (dynamic_cast<Int8ImageType*>(m_Image.GetPointer()))
    {
    SwitchImageWrite<Int8ImageType>();
    }
  else if (dynamic_cast<UInt8ImageType*>(m_Image.GetPointer()))
    {
    SwitchImageWrite<UInt8ImageType>();
    }
  else if (dynamic_cast<Int16ImageType*>(m_Image.GetPointer()))
    {
    SwitchImageWrite<Int16ImageType>();
    }
  else if (dynamic_cast<UInt16ImageType*>(m_Image.GetPointer()))
    {
    SwitchImageWrite<UInt16ImageType>();
    }
  else if (dynamic_cast<Int32ImageType*>(m_Image.GetPointer()))
    {
    SwitchImageWrite<Int32ImageType>();
    }
  else if (dynamic_cast<UInt32ImageType*>(m_Image.GetPointer()))
    {
    SwitchImageWrite<UInt32ImageType>();
    }
  else if (dynamic_cast<FloatImageType*>(m_Image.GetPointer()))
    {
    SwitchImageWrite<FloatImageType>();
    }
  else if (dynamic_cast<DoubleImageType*>(m_Image.GetPointer()))
    {
    SwitchImageWrite<DoubleImageType>();
    }
  else if (dynamic_cast<Int8VectorImageType*>(m_Image.GetPointer()))
    {
    SwitchVectorImageWrite<Int8VectorImageType>();
    }
  else if (dynamic_cast<UInt8VectorImageType*>(m_Image.GetPointer()))
    {
    SwitchVectorImageWrite<UInt8VectorImageType>();
    }
  else if (dynamic_cast<Int16VectorImageType*>(m_Image.GetPointer()))
    {
    SwitchVectorImageWrite<Int16VectorImageType>();
    }
  else if (dynamic_cast<UInt16VectorImageType*>(m_Image.GetPointer()))
    {
    SwitchVectorImageWrite<UInt16VectorImageType>();
    }
  else if (dynamic_cast<Int32VectorImageType*>(m_Image.GetPointer()))
    {
    SwitchVectorImageWrite<Int32VectorImageType>();
    }
  else if (dynamic_cast<UInt32VectorImageType*>(m_Image.GetPointer()))
    {
    SwitchVectorImageWrite<UInt32VectorImageType>();
    }
  else if (dynamic_cast<FloatVectorImageType*>(m_Image.GetPointer()))
    {
    SwitchVectorImageWrite<FloatVectorImageType>();
    }
  else if (dynamic_cast<DoubleVectorImageType*>(m_Image.GetPointer()))
    {
    SwitchVectorImageWrite<DoubleVectorImageType>();
    }
  else if (dynamic_cast<UInt8RGBImageType*>(m_Image.GetPointer()))
    {
    SwitchRGBImageWrite<UInt8RGBImageType>();
    }
  else if (dynamic_cast<UInt8RGBAImageType*>(m_Image.GetPointer()))
    {
    SwitchRGBAImageWrite<UInt8RGBAImageType>();
    }
  else
    {
    itkExceptionMacro("Unknown image type");
    }
  }


itk::ProcessObject*
OutputImageParameter::GetWriter()
{
  int type = 0;
  // 0 : image
  // 1 : VectorImage
  // 2 : RGBAImage
  // 3 : RGBImage
  itk::ProcessObject* writer = 0;
  if (dynamic_cast<Int8VectorImageType*> (m_Image.GetPointer())
      || dynamic_cast<UInt8VectorImageType*> (m_Image.GetPointer())
      || dynamic_cast<Int16VectorImageType*> (m_Image.GetPointer())
      || dynamic_cast<UInt16VectorImageType*> (m_Image.GetPointer())
      || dynamic_cast<Int32VectorImageType*> (m_Image.GetPointer())
      || dynamic_cast<UInt32VectorImageType*> (m_Image.GetPointer())
      || dynamic_cast<FloatVectorImageType*> (m_Image.GetPointer())
      || dynamic_cast<DoubleVectorImageType*> (m_Image.GetPointer()))
    {
    type = 1;
    }
  else
    if (dynamic_cast<UInt8RGBAImageType*> (m_Image.GetPointer()))
      {
      type = 2;
      writer = m_RGBAUInt8Writer;
      itkWarningMacro("UInt8RGBAImageType will be saved in UInt8 format.");
      return writer;
      }
    else
      if (dynamic_cast<UInt8RGBImageType*> (m_Image.GetPointer()))
        {
        type = 3;
        writer = m_RGBUInt8Writer;
        itkWarningMacro("UInt8RGBImageType will be saved in UInt8 format.");
        return writer;
        }


  switch (GetPixelType())
    {
    case ImagePixelType_int8:
      {
      if (type == 1)
        writer = m_VectorInt8Writer;
      else
        if (type == 0) writer = m_Int8Writer;

      break;
      }
    case ImagePixelType_uint8:
      {
      if (type == 1)
        writer = m_VectorUInt8Writer;
      else
        if (type == 0)
          writer = m_UInt8Writer;
        else
          if (type == 2)
            writer = m_RGBAUInt8Writer;
          else writer = m_RGBUInt8Writer;
      break;
      }
    case ImagePixelType_int16:
      {
      if (type == 1)
        writer = m_VectorInt16Writer;
      else
        if (type == 0) writer = m_Int16Writer;
      break;
      }
    case ImagePixelType_uint16:
      {
      if (type == 1)
        writer = m_VectorUInt16Writer;
      else
        if (type == 0) writer = m_UInt16Writer;
      break;
      }
    case ImagePixelType_int32:
      {
      if (type == 1)
        writer = m_VectorInt32Writer;
      else
        if (type == 0) writer = m_Int32Writer;
      break;
      }
    case ImagePixelType_uint32:
      {
      if (type == 1)
        writer = m_VectorUInt32Writer;
      else
        if (type == 0) writer = m_UInt32Writer;
      break;
      }
    case ImagePixelType_float:
      {
      if (type == 1)
        writer = m_VectorFloatWriter;
      else
        if (type == 0) writer = m_FloatWriter;
      break;
      }
    case ImagePixelType_double:
      {
      if (type == 1)
        writer = m_VectorDoubleWriter;
      else
        if (type == 0) writer = m_DoubleWriter;
      break;
      }
    }
  if (0 == writer)
    {
    itkExceptionMacro("Unknown Writer type.");
    }

  return writer;
}

OutputImageParameter::ImageBaseType*
OutputImageParameter::GetValue( )
{
  return m_Image;
}

void
OutputImageParameter::SetValue(ImageBaseType* image)
{
  m_Image = image;
  SetActive(true);
}

bool
OutputImageParameter::HasValue() const
{
  std::string filename(this->GetFileName());
  return !filename.empty();
}

}
}
