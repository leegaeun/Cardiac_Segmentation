/*=========================================================================

Program:	Label Fusion with Majority Voting.
Module:		LabelFusionWithMajorityVoting.cxx
Language:	C++
Date:		2020-01-12 13:54:00
Version:	VER 0.1
Developer:	June-Goo Lee (junegoo.lee@gmail.com), Gaeun Lee (ggelee93@gmail.com)

Copyright (c) Medical Imaging and Intelligent Reality Lab (MI2RL) in Asan Medical Center (AMC).
All rights reserved.

[TEST_EXAMPLE]
Preparation.exe ./image.mha ./image_dis1.mha 1

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <math.h>
#define _USE_MATH_DEFINES

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

// Iterator
#include "itkMedianImageFilter.h"

#include "itkSTAPLEImageFilter.h"

#include <string>
#include <vector>

typedef float                                            FloatPixelType;
typedef short                                           PixelType;
typedef unsigned char                            MaskPixelType;
typedef itk::RGBPixel< unsigned char > RGBPixelType;

typedef itk::Image< FloatPixelType, 3 >    FloatImageType;
typedef itk::Image< PixelType, 3 >            ImageType;
typedef itk::Image< MaskPixelType, 3 >   MaskImageType;
typedef itk::Image< RGBPixelType, 3 >     RGBImageType;

typedef itk::Image< PixelType, 2 >            ImageType2D;
typedef itk::Image< MaskPixelType, 2 >   MaskImageType2D;



void MakeMaskImageFile(MaskImageType::Pointer image, const char* filename)
{
  typedef itk::ImageFileWriter<MaskImageType>		WriterType;

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(image);
  writer->SetFileName(filename);

  writer->Update();
}

void MedianFilter(MaskImageType::Pointer label, int kernel_width, int kernel_height, int kernel_z_radius)
{
  typedef itk::ImageRegionConstIterator< MaskImageType > ConstMaskImageIteratorType;
  typedef itk::ImageRegionIterator< MaskImageType > MaskImageIteratorType;

  typedef itk::MedianImageFilter< MaskImageType, MaskImageType >   MedianFilterType;

  MedianFilterType::Pointer median = MedianFilterType::New();
  MedianFilterType::InputSizeType radius;
  radius[0] = kernel_width;
  radius[1] = kernel_height;
  radius[2] = kernel_z_radius;
  median->SetRadius(radius);
  median->SetInput(label);
  median->Update();

  ConstMaskImageIteratorType in(median->GetOutput(), median->GetOutput()->GetBufferedRegion());
  MaskImageIteratorType out(label, label->GetBufferedRegion());

  for (in.GoToBegin(), out.GoToBegin(); !in.IsAtEnd(); ++in, ++out)
  {
    out.Set(in.Get());
  }
}

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " [Input_Filename1  Input_Filename2 ...]   Output_Filename ";
    std::cerr << std::endl;
    return EXIT_FAILURE;
  }

  std::list<std::string> m_listFilenames;
  std::string tmpStr;
  for (int i = 1; i < argc-1; i++)
  {
	  m_listFilenames.push_back(argv[i]);
  }

  typedef itk::ImageFileReader< MaskImageType  >  MaskReaderType;

  MaskReaderType::Pointer reader = MaskReaderType::New();
  reader->SetFileName(m_listFilenames.front().c_str());

  try
  {
    reader->Update();
  }
  catch (itk::ExceptionObject &ex)
  {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
  }

  MaskImageType::Pointer label = reader->GetOutput();
  MaskImageType::SizeType size = label->GetLargestPossibleRegion().GetSize();
  
  // Make average image (voxel: probability for each label) 
  std::map<int, std::vector<double>>  probImageMap;
  std::vector<double> tmpVec;
  tmpVec.resize(8);
  tmpVec[0] = tmpVec[1] = tmpVec[2] = tmpVec[3] = tmpVec[4] = tmpVec[5] = tmpVec[6] = tmpVec[7] = 0.0;

  for (int i = 0; i<size[0] * size[1] * size[2]; ++i)
    probImageMap[i] = tmpVec;

  std::list<std::string>::iterator iterList;
  for (iterList = m_listFilenames.begin(); iterList != m_listFilenames.end(); iterList++)
  {
    MaskReaderType::Pointer tmp_reader = MaskReaderType::New();
    tmp_reader->SetFileName(iterList->c_str());
    tmp_reader->Update();

    MaskPixelType* lpLabelData = tmp_reader->GetOutput()->GetBufferPointer();
    for (int i = 0; i < size[0] * size[1] * size[2]; ++i)
    {
      probImageMap[i][lpLabelData[i]] += 1.0;
    }
  }

  // Make label image from calcuated probability image map
  MaskImageType::Pointer output_label = MaskImageType::New();
  output_label->SetRegions(label->GetLargestPossibleRegion());
  output_label->CopyInformation(label);
  output_label->Allocate();
  
  MaskPixelType* lpMaskData = output_label->GetBufferPointer();

  double max;
  int loc;
  for (int i = 0; i<size[0] * size[1] * size[2]; ++i)
  {
    tmpVec = probImageMap[i];

    max = -1000000.0;
    loc = 0;
    for (int j = 0; j<8; ++j)
    {
      if (tmpVec[j] > max)
      {
        loc = j;
        max = tmpVec[j];
      }
    }

    lpMaskData[i] = loc;
    
    // if there is any meniscus at this voxel, mark it as meniscus 
    if (tmpVec[7] > 0.0)
      lpMaskData[i] = 7;

  }

  MedianFilter(output_label, 1, 1, 0);

  MakeMaskImageFile(output_label, argv[argc - 1]);
  
  return EXIT_SUCCESS;
}