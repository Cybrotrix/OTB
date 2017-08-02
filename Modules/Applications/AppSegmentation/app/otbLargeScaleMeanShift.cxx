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

#include "otbWrapperCompositeApplication.h"
#include "otbWrapperApplicationFactory.h"

namespace otb
{
namespace Wrapper
{

/**
 * \class LargeScaleMeanShift
 *
 * \brief All-in-one application for the LSMS framework
 *
 * This application gathers the 4 steps of the large-scale MeanShift
 * segmentation framework.
 * 
 */
class LargeScaleMeanShift : public CompositeApplication
{
public:
  /** Standard class typedefs. */
  typedef LargeScaleMeanShift           Self;
  typedef CompositeApplication          Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

/** Standard macro */
  itkNewMacro(Self);

  itkTypeMacro(LargeScaleMeanShift, otb::CompositeApplication);

private:
  void DoInit() ITK_OVERRIDE
    {
    SetName("LargeScaleMeanShift");
    SetDescription("Large-scale segmentation using MeanShift");

    // Documentation
    SetDocName("Large-Scale MeanShift");
    SetDocLongDescription("This application chains together the 4 steps of the "
      "MeanShit framework, that is the MeanShiftSmoothing [1], the "
      "LSMSSegmentation [2], the LSMSSmallRegionsMerging [3] and the "
      "LSMSVectorization [4].\n\n"
      "It generates a vector data file containing the regions extracted with "
      "the MeanShift algorithm.\n\n"
      "This application can be a preliminary step for an object-based analysis."
      );
    SetDocLimitations("None");
    SetDocAuthors("OTB-Team");
    SetDocSeeAlso("[1] MeanShiftSmoothing\n"
      "[2] LSMSSegmentation\n"
      "[3] LSMSSmallRegionsMerging\n"
      "[4] LSMSVectorization");

    AddDocTag(Tags::Segmentation);
    AddDocTag("LSMS");

    ClearApplications();
    AddApplication("MeanShiftSmoothing", "smoothing", "Smoothing step");
    AddApplication("LSMSSegmentation", "segmentation", "Segmentation step");
    AddApplication("LSMSSmallRegionsMerging", "merging", "Small region merging step");
    AddApplication("LSMSVectorization", "vectorization", "Vectorization step");

    ShareParameter("in","smoothing.in");
    ShareParameter("spatialr","smoothing.spatialr");
    ShareParameter("ranger","smoothing.ranger");
    ShareParameter("minsize","merging.minsize");

    ShareParameter("tilesizex","segmentation.tilesizex");
    ShareParameter("tilesizey","segmentation.tilesizey");

    AddParameter(ParameterType_InputImage, "imfield", "Support image for field computation");
    SetParameterDescription( "imfield", "This is an optional support image "
      "that can be used to compute field values in each region." );
    MandatoryOff("imfield");

    ShareParameter("out","vectorization.out");

    AddParameter( ParameterType_Empty, "cleanup", "Temporary files cleaning" );
    EnableParameter( "cleanup" );
    SetParameterDescription( "cleanup",
      "If activated, the application will try to clean all temporary files it created" );
    MandatoryOff( "cleanup" );

    // Setup RAM
    ShareParameter("ram","smoothing.ram");
    Connect("merging.ram","smoothing.ram");
    Connect("vectorization.ram","smoothing.ram");

    Connect("merging.tilesizex","segmentation.tilesizex");
    Connect("merging.tilesizey","segmentation.tilesizey");
    Connect("vectorization.tilesizex","segmentation.tilesizex");
    Connect("vectorization.tilesizey","segmentation.tilesizey");

    // TODO : this is not exactly true, we used to choose the smoothed image instead
    Connect("merging.in","smoothing.in");

    // Setup constant parameters
    GetInternalApplication("smoothing")->SetParameterString("foutpos","foo");
    GetInternalApplication("smoothing")->EnableParameter("foutpos");

    // Doc example parameter settings
    SetDocExampleParameterValue("in", "QB_1_ortho.tif");
    SetDocExampleParameterValue("spatialr", "4");
    SetDocExampleParameterValue("ranger", "80");
    SetDocExampleParameterValue("minsize", "16");
    SetDocExampleParameterValue("out", "regions.shp");

    SetOfficialDocLink();
    }

  void DoUpdateParameters() ITK_OVERRIDE
  {}

  void DoExecute() ITK_OVERRIDE
    {
    std::vector<std::string> tmpFilenames;
    tmpFilenames.push_back(GetParameterString("out")+std::string("_labelmap.tif"));
    tmpFilenames.push_back(GetParameterString("out")+std::string("_labelmap_merged.tif"));
    tmpFilenames.push_back(GetParameterString("out")+std::string("_labelmap.geom"));
    tmpFilenames.push_back(GetParameterString("out")+std::string("_labelmap_merged.geom"));
    ExecuteInternal("smoothing");
    // in-memory connexion here (saves 1 additional update for foutpos)
    GetInternalApplication("segmentation")->SetParameterInputImage("in",
      GetInternalApplication("smoothing")->GetParameterOutputImage("fout"));
    GetInternalApplication("segmentation")->SetParameterInputImage("inpos",
      GetInternalApplication("smoothing")->GetParameterOutputImage("foutpos"));
    // temporary file output here
    GetInternalApplication("segmentation")->SetParameterString("out",
      tmpFilenames[0]);
    // take half of previous radii
    GetInternalApplication("segmentation")->SetParameterFloat("spatialr",
      0.5 * (double)GetInternalApplication("smoothing")->GetParameterInt("spatialr"));
    GetInternalApplication("segmentation")->SetParameterFloat("ranger",
      0.5 * GetInternalApplication("smoothing")->GetParameterFloat("ranger"));
    GetInternalApplication("segmentation")->ExecuteAndWriteOutput();

    GetInternalApplication("merging")->SetParameterString("inseg",
      tmpFilenames[0]);
    GetInternalApplication("merging")->SetParameterString("out",
      tmpFilenames[1]);
    GetInternalApplication("merging")->ExecuteAndWriteOutput();

    if (IsParameterEnabled("imfield") && HasValue("imfield"))
      {
      GetInternalApplication("vectorization")->SetParameterString("in",
        GetParameterString("imfield"));
      }
    else
      {
      GetInternalApplication("vectorization")->SetParameterString("in",
        GetParameterString("in"));
      }
    GetInternalApplication("vectorization")->SetParameterString("inseg",
      tmpFilenames[1]);
    ExecuteInternal("vectorization");

    if( IsParameterEnabled( "cleanup" ) )
      {
      otbAppLogINFO( <<"Final clean-up ..." );
      for (unsigned int i=0 ; i<tmpFilenames.size() ; ++i)
        {
        if(itksys::SystemTools::FileExists(tmpFilenames[i].c_str()))
          {
          itksys::SystemTools::RemoveFile(tmpFilenames[i].c_str());
          }
        }
      }
    }

};

} // end of namespace Wrapper
} // end of namespace otb

OTB_APPLICATION_EXPORT(otb::Wrapper::LargeScaleMeanShift)