#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <fstream>

// This is used for the PL Kernels
// XRT includes
#include "xrt/experimental/xrt_bo.h"
#include "xrt/experimental/xrt_xclbin.h"
#include "xrt/experimental/xrt_device.h"
#include "xrt/experimental/xrt_kernel.h"
#include "xrt/experimental/xrt_ip.h"
#include<iostream>
#include"bmpFile.h"

#define SAMPLES 256

void wait_for_enter(const std::string &msg) {
    std::cout << msg << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void add_iter(int stream_len, int iter_times, uint16_t* data_in, uint16_t* data_out){
	for(int i=0; i<stream_len; i++){
		data_out[i] = 0;
		for(int j=0; j<iter_times; j++){
			data_out[i] = data_out[i] + data_in[j*stream_len + i];
		}
	}
}

int main(int argc, char ** argv)
{
	//////////////////////////////////////////
	// Open xclbin
	//////////////////////////////////////////
	//int stream_len = 32;
	//int iter_times = 8;

	std::string binaryFile = argv[1];
  std::string imageNum = argv[2];
  std::string imageDir = argv[3];
  std::string outFileName = argv[4];

  std::cout << "C1" << std::endl;

  auto xclbin = xrt::xclbin(binaryFile);
	int device_index = 0;

	std::cout << "Open the device" << device_index << std::endl;
  auto device = xrt::device(device_index);
  std::cout << "Load the xclbin " << binaryFile << std::endl;
	auto uuid = device.load_xclbin(binaryFile);
	std::cout << "Finsh Load the xclbin " << binaryFile << std::endl;
	
	auto mm2s = xrt::kernel(device, uuid, "mm2s");
	auto s2mm = xrt::kernel(device, uuid, "s2mm");
 auto UpsamplingIP = xrt::ip(device,uuid,"SuperResolutionCore");
 
	//auto adderIP = xrt::ip(device, uuid, "AddIterTop");

	wait_for_enter("\nPress ENTER to continue after setting up ILA trigger...");

  bmp image;
  image.readPic(imageDir);
  int inputImgH = image.getBmpFileInFoHeader().biHeight;
  int inputImgW = image.getBmpFileInFoHeader().biWidth;
  auto inputImgDataSizeC1 = inputImgH * inputImgW;
  auto inputImgDataSizeC3 = inputImgDataSizeC1 * 3;
  
  std::cout << "inputImgH:" << inputImgH <<std::endl;
  std::cout << "inputImgW:" << inputImgW <<std::endl;
  std::cout << "inputImgDataSizeC1:" << inputImgDataSizeC1 << std::endl;
  std::cout << "inputImgDataSizeC3:" << inputImgDataSizeC3 << std::endl;
  
  //output image constrution
  int outputImgW = inputImgW * 4;
  int outputImgH = inputImgH * 4;  //*4
  auto outputImgDataSizeC1 = inputImgDataSizeC1 * 16;//* 16
  auto outputImgDataSizeC3 = outputImgDataSizeC1 * 3 ;
  unsigned int Bfsize = sizeof(BmpFileHeader) + sizeof(BmpFileInFoHeader) + outputImgDataSizeC1 * 3;
  BmpFileHeader outFileHeader = {BMPTYPE, Bfsize, 0, 0, 54};
  BmpFileInFoHeader outFileInFoHeader = {40, outputImgW, outputImgH, 1, 24, 0, outputImgDataSizeC1*3, 0, 0, 0, 0};
  auto outputSurface = new RGBTriple[outputImgDataSizeC3];
  int outputoffset = (outputImgW * 3) % 4;
  
  std::cout << "outputImgW:" << outputImgH <<std::endl;
  std::cout << "outputImgW:" << outputImgW <<std::endl;
  std::cout << "outputImgDataSizeC1:" << outputImgDataSizeC1 << std::endl;
  std::cout << "outputImgDataSizeC3:" << outputImgDataSizeC3 << std::endl;
  std::cout << "BfSize of output image:" << Bfsize << std::endl;
  
  auto boASize = inputImgDataSizeC1 * 4;
  auto boBSize = outputImgDataSizeC1 * 4;
	//////////////////////////////////////////
  // Create Buffer
  //////////////////////////////////////////
	//size_t vector_size_bytes = sizeof(uint16_t) * SAMPLES;
	std::cout << "Allocate Buffer in Global Memory\n";
	auto boA = xrt::bo(device,  boASize, mm2s.group_id(0)); //Match kernel arguments to RTL kernel
  auto boB = xrt::bo(device, boBSize, s2mm.group_id(0));
  

	// Map the contents of the buffer object into host memory
	
  auto boA_map = boA.map<uint8_t*>();
  auto boB_map = boB.map<uint8_t*>();
  std::fill(boA_map, boA_map + boASize, 0);
  std::fill(boB_map, boB_map + boBSize, 0);
 
  int RGBTripleIndex = 0;
  int RGB = 0;
	for (int i = 0; i < boASize; ++i) {
         RGB = i%4;
         if(RGB == 0)
	       { boA_map[i] = image.getRGBTriple()[RGBTripleIndex].blue;}
         if(RGB == 1)
         { boA_map[i] = image.getRGBTriple()[RGBTripleIndex].green;}
         if(RGB == 2)
         { boA_map[i] = image.getRGBTriple()[RGBTripleIndex].red;}
         if(RGB == 3)
         {
           boA_map[i] = 0;
           RGBTripleIndex = RGBTripleIndex + 1;
         }
         
  }

	std::cout << "synchronize input buffer data to device global memory\n";
	boA.sync(XCL_BO_SYNC_BO_TO_DEVICE);
  boB.sync(XCL_BO_SYNC_BO_TO_DEVICE);

	std::cout << "Execution of the kernel\n";
	auto mm2s_run = mm2s(boA, nullptr, inputImgDataSizeC1); //DATA_SIZE=size
 
  UpsamplingIP.write_register(0x04,960);
  UpsamplingIP.write_register(0x08,540);
  UpsamplingIP.write_register(0x0c,150);
  UpsamplingIP.write_register(0x10,1);
  //adderIP.write_register(0x04, 0);
 
	auto s2mm_run = s2mm(boB, nullptr, outputImgDataSizeC1); //DATA_SIZE=size

 

	std::cout << "1-kernel starting, wait for the mm2s kernel end \n";
	mm2s_run.wait();

	std::cout << "mm2s kernel end, wait for the s2mm kernel end \n";
  s2mm_run.wait();
  


	// Get the output;
  std::cout << "Get the output data from the device" << std::endl;
  boB.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

  RGBTripleIndex = 0;
  RGB = 0;
  for (int i = 0;i <boBSize;i++) {
           RGB = i%4;
           if(RGB == 0)
           { outputSurface[RGBTripleIndex].blue = boB_map[i]; }
           if(RGB == 1)  
           { outputSurface[RGBTripleIndex].green = boB_map[i]; }
           if(RGB == 2)
           { outputSurface[RGBTripleIndex].red = boB_map[i]; }
           if(RGB == 3)
           {RGBTripleIndex = RGBTripleIndex + 1;}
  }
  
  bmp outputBmp(outputoffset, outputSurface, outFileHeader, outFileInFoHeader);
  outputBmp.writePic(outFileName);

	bool cmp_result = true;
	for (size_t i = 0; i < 100 ; ++i) {
    printf("the value of %ld-input is: %d \n", i,boA_map[i]);
		printf("the value of %ld-out is: %d \n", i,boB_map[i]);

		if(boA_map[i] != boB_map[i]) cmp_result = false;
  }

	if(cmp_result) printf("TEST PASS! \n");
	else printf("TEST FAIL!");


	return 0;


}