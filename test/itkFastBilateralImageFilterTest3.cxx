/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include <fstream>
#include "itkFastBilateralImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "itkTestingMacros.h"


/**
 * This test was originally taken from the tests for the itkBilateralImageFilter
 * and modified for the itkFastBilateralImageFilter.
 */
int
itkFastBilateralImageFilterTest3(int ac, char * av[])
{
  if (ac < 3)
  {
    std::cerr << "Usage: " << av[0] << " InputImage BaselineImage\n";
    return -1;
  }

  using PixelType = unsigned char;
  using ImageType = itk::Image<PixelType, 2>;
  auto input = itk::ImageFileReader<ImageType>::New();
  input->SetFileName(av[1]);

  // Create a filter
  using FilterType = itk::FastBilateralImageFilter<ImageType, ImageType>;

  FilterType::Pointer filter1 = FilterType::New();
  filter1->SetInput(input->GetOutput());
  FilterType::Pointer filter2 = FilterType::New();
  filter2->SetInput(filter1->GetOutput());
  FilterType::Pointer filter3 = FilterType::New();
  filter3->SetInput(filter2->GetOutput());

  // Instead of using a single agressive smoothing filter, use 3
  // less aggressive filters.
  //
  // These settings match the "wedding" cake image (cake_easy.png) where
  // the signal to noise ratio is 5 (step heights near 100 units,
  // noise sigma near 20 units). A single filter stage with these
  // settings cuts the noise level in half.  These three stages should
  // reduce the amount of noise by a factor of 8. This is comparable to
  // the noise reduction in using a single stage with parameters
  // (4.0, 50.0).  The difference is that with 3 less aggressive stages
  // the edges are preserved better.
  filter1->SetDomainSigma(4.0);
  filter1->SetRangeSigma(20.0);
  filter2->SetDomainSigma(4.0);
  filter2->SetRangeSigma(20.0);
  filter3->SetDomainSigma(4.0);
  filter3->SetRangeSigma(20.0);

  try
  {
    input->Update();
    filter3->Update();
  }
  catch (itk::ExceptionObject & e)
  {
    std::cerr << "Exception detected: " << e.GetDescription();
    return -1;
  }

  // Generate test image
  itk::ImageFileWriter<ImageType>::Pointer writer;
  writer = itk::ImageFileWriter<ImageType>::New();
  writer->SetInput(filter3->GetOutput());
  writer->SetFileName(av[2]);
  writer->Update();

  return EXIT_SUCCESS;
}
