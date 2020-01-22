/*=========================================================================

Program:	Rescale & Apply displacement.
Module:		DataPreparation.cxx
Language:	C++
Date:		2020-01-10 11:04:00
Version:	VER 0.1
Developer:	June-Goo Lee (junegoo.lee@gmail.com), Gaeun Lee (ggelee93@gmail.com)

Copyright (c) Medical Imaging and Intelligent Reality Lab (MI2RL) in Asan Medical Center (AMC).
All rights reserved.

[TEST_EXAMPLE]
Preparation.exe ./image.mha ./image_dis1.mha 1
Preparation.exe ./image.mha ./image_dis1_50wl350ww.mha 1 50 350

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <math.h>
#define _USE_MATH_DEFINES

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include <string>
#include <vector>

typedef float	FloatPixelType;
typedef short	InputPixelType;
typedef unsigned char	OutputPixelType;
typedef itk::RGBPixel< OutputPixelType > RGBPixelType;

typedef itk::Image< InputPixelType, 3 >	ImageType;
typedef itk::Image< RGBPixelType, 3 >	RGBImageType;

typedef itk::Image< InputPixelType, 2 >	ImageType2D;


void MakeRGBImageFile(RGBImageType::Pointer image, const char* filename)
{
  typedef itk::ImageFileWriter<RGBImageType>		WriterType;

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(image);
  writer->SetFileName(filename);

  writer->Update();
}

void CalculateWWWL(std::map<int, unsigned int>& histogram, int min, int max, double lowP, double highP, int& WW, int& WL)
{
  double sum = 0.0;
  for (int i = min; i <= max; i++)
  {
    sum += histogram[i];
  }

  double partial_sum = 0.0;
  int min_index = min;
  int index = min;
  while (partial_sum < lowP*sum)
  {
    partial_sum += histogram[index];
    index++;
  }
  min_index = index;

  partial_sum = 0.0;
  int max_index = max;
  index = max;
  while (partial_sum < highP*sum)
  {
    partial_sum += histogram[index];
    index--;
  }
  max_index = index;

  WW = max_index - min_index + 1;
  WL = (max_index + min_index) / 2;
}

int main(int argc, char* argv[])
{
  if (argc < 4)
  {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " Input_Filename   Output_Filename   STEP " ;
	std::cerr << " (options) WL   WW ";
    std::cerr << std::endl;
    return EXIT_FAILURE;
  }

  const char* input_filename = argv[1];
  const char* output_filename = argv[2];
  const int iSTEP = atoi(argv[3]);

  typedef itk::ImageFileReader< ImageType  >  ReaderType;
  ReaderType::Pointer reader = ReaderType::New();

  reader->SetFileName(input_filename);

  try
  {
    reader->Update();
  }
  catch (itk::ExceptionObject &ex)
  {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
  }

  ImageType::Pointer image = reader->GetOutput();
  ImageType::SizeType size = image->GetBufferedRegion().GetSize();
  ImageType::SpacingType spacing = image->GetSpacing();

  // [STEP1] Convert 12bit to 8bit.
  InputPixelType* lpData = image->GetBufferPointer();

  int min, max;
  min = 1000000;   max = -1000000;
  int index;
  for (int z = 0; z<size[2]; z++)
  {
    for (int y = 0; y<size[1]; y++)
    {
      for (int x = 0; x<size[0]; x++)
      {
        index = z*size[1] * size[0] + y*size[0] + x;
        if (min > lpData[index])
          min = lpData[index];
        if (max < lpData[index])
          max = lpData[index];
      }
    }
  }

  std::map<int, unsigned int> histogram;
  for (int i = min; i <= max; i++)
    histogram[i] = 0;

  for (int z = 0; z<size[2]; z++)
  {
    for (int y = 0; y<size[1]; y++)
    {
      for (int x = 0; x<size[0]; x++)
      {
        index = z*size[1] * size[0] + y*size[0] + x;
        histogram[lpData[index]] += 1;
      }
    }
  }

  int WL, WW;
  if (argc > 5){
	  WL = atoi(argv[4]);
	  WW = atoi(argv[5]);
  }
  else{
	  CalculateWWWL(histogram, min, max, 0.15, 0.005, WW, WL);
  }

  float fTemp;
  for (int i = 0; i < size[0] * size[1] * size[2]; i++)
  {
    fTemp = lpData[i];
    fTemp = 255.0*(fTemp - WL + 0.5*WW) / WW;
    if (fTemp >= 255.0)
		lpData[i] = 255;
    else if (fTemp <= 0.0)
		lpData[i] = 0;
    else
		lpData[i] = (OutputPixelType)fTemp;
  }


  // [STEP2] Apply displacement (iSTEP).
  int index_r, index_g, index_b;
  char tmp_filename[4096];
  
  RGBImageType::Pointer rgb_image = RGBImageType::New();
  rgb_image->SetRegions(image->GetLargestPossibleRegion());
  rgb_image->SetSpacing(spacing);
  rgb_image->Allocate();

  RGBPixelType* lpRGBData = rgb_image->GetBufferPointer();

  for (int z = 0; z < size[2]; z++)
  {
    for (int y = 0; y < size[1]; y++)
    {
      for (int x = 0; x < size[0]; x++)
      {
        index = z*size[1] * size[0] + y*size[0] + x;

        if (z < iSTEP)
        {
          index_r = (0)*size[1] * size[0] + y*size[0] + x;
          index_g = z*size[1] * size[0] + y*size[0] + x;
          index_b = (z + iSTEP)*size[1] * size[0] + y*size[0] + x;
        }
        else if (z >= size[2] - iSTEP)
        {
          index_r = (z - iSTEP)*size[1] * size[0] + y*size[0] + x;
          index_g = z*size[1] * size[0] + y*size[0] + x;
          index_b = (size[2] - 1)*size[1] * size[0] + y*size[0] + x;
        }
        else
        {
          index_r = (z - iSTEP)*size[1] * size[0] + y*size[0] + x;
          index_g = z*size[1] * size[0] + y*size[0] + x;
          index_b = (z + iSTEP)*size[1] * size[0] + y*size[0] + x;
        }

		lpRGBData[index].SetRed((OutputPixelType)lpData[index_r]);
		lpRGBData[index].SetGreen((OutputPixelType)lpData[index_g]);
		lpRGBData[index].SetBlue((OutputPixelType)lpData[index_b]);

      }
    }
  }

  MakeRGBImageFile(rgb_image, output_filename);

  return EXIT_SUCCESS;
}