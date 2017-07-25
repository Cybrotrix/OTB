/*
 * Copyright (C) 2005-2017 Centre National d'Etudes Spatiales (CNES)
 *
 * This file is part of Orfeo Toolbox
 *
 *     https://www.orfeo-toolbox.org/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef otbContrastEnhancementFilter_h
#define otbContrastEnhancementFilter_h

#include "itkImageToImageFilter.h"
#include "itkImage.h"

#include "itkMultiplyImageFilter.h"

namespace otb
{



template <class TInputImage, class TOutputImage , int Tsize >
class ITK_EXPORT ContrastEnhancementFilter :
	public itk::ImageToImageFilter< TInputImage , TOutputImage >
{
public:

	/** typedef to simplify variables definition and declaration. */
  typedef TInputImage InputImageType;
  /** typedef to simplify variables definition and declaration. */
  typedef TOutputImage OutputImageType;

	/** typedef for standard classes. */
	typedef ContrastEnhancementFilter Self;
  typedef itk::ImageToImageFilter< InputImageType, OutputImageType > Superclass;
  typedef itk::SmartPointer< Self > Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  typedef itk::Image< float , 2 > ImageGainType; 

  /** "object factory" management method. */
  itkNewMacro(Self);
  /** Return the class name. */
  itkTypeMacro(ContrastEnhancementFilter, ImageToImageFilter);

  void 
	equalized( const std::array< int , Tsize > & inputHisto,
	           std::array< int , Tsize > & lut);
	
	void
	equalized( std::array< int , Tsize > gridHisto[] , 
						 std::array< int , Tsize > gridLut[] ,
						 int nW ,
						 int nH );
	
	void
	computehisto( typename TInputImage::ConstPointer const input ,
								std::array< int , Tsize > gridHisto[] ,
								int nW ,
								int nH );

	void
	createTarget( typename TInputImage::ConstPointer const input );

	void
	createTarget( int h , int l);

	float
	interpoleGain( const std::array< int , Tsize > gridLut[] ,
								 int pixelValue ,
                 typename TInputImage::IndexType index ,
                 int nW ,
                 int nH );

	void
	histoLimiteContrast( std::array< int , Tsize > gridHisto[] ,
											 int hThresh ,
											 int nW ,
											 int nH );
	
	void
	gainLimiteContrast();

	void
	setThumbnailSize( int hThumbnaili , int wThumbnaili )
	{
		this->hThumbnail = hThumbnaili;
		this->wThumbnail = wThumbnaili;
	}

	void
	setHistoThreshFactor( float threshFactori )
	{
		this->threshFactor = threshFactori;
	}

	void
	setGainThresh( float lowThreshi , float upThreshi)
	{
		assert( lowThreshi<=1 && lowThreshi >=0 );
		assert( upThreshi>=1);
		this->upThresh = upThreshi;
		this->lowThresh = lowThreshi;
	}

protected:
 	ContrastEnhancementFilter();
 	~ContrastEnhancementFilter() ITK_OVERRIDE {}
  // void PrintSelf(std::ostream& os, itk::Indent indent) const ITK_OVERRIDE;

 	typedef itk::MultiplyImageFilter< InputImageType, ImageGainType, OutputImageType > MultiplyImageFilterType;
 	typename MultiplyImageFilterType::Pointer gainMultiplyer;

 	ImageGainType::Pointer gainImage;
 	std::array< int , Tsize > targetHisto;
 	int wThumbnail;
 	int hThumbnail;
 	float threshFactor;
 	float lowThresh;
 	float upThresh;


 	void GenerateData();

private:
  ContrastEnhancementFilter(const Self &); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

};


}

#ifndef OTB_MANUAL_INSTANTIATION
#include "otbContrastEnhancementFilter.txx"
#endif

#endif