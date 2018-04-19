#include <iostream>
#include <fstream>
#include <iomanip>
#include <time.h>

#include "modelA.h"
#include "compressor.h"
#include "decompressor.h"
int getFileSize(const std::string &fileName)
{
    std::ifstream file(fileName.c_str(), std::ifstream::in | std::ifstream::binary);

    if(!file.is_open())
    {
        return -1;
    }

    file.seekg(0, std::ios::end);
    int fileSize = file.tellg();
    file.close();

    return fileSize;
}
int main(int argc, char* argv[])
{
	int cflag = 0;
	int lflag = 0;
	int i = 1;
	for(; i < argc-(2-cflag); i++){
		if (!strcmp(argv[i], "-d")){
			i++;
			if (i+1 >= argc){
				fprintf(stderr,"No filenames\n");
				return 1;
			}
			try {
				std::ifstream input(argv[i], std::ifstream::binary);
				i++;
				std::ofstream output(argv[i], std::ofstream::binary);
				modelA<int, 16, 14> cmodel;

				std::cout << "decompressing...\n";
				decompress(input, output, cmodel);
				return 0;
			}
			catch (std::exception &ex){
				std::cerr << "Failed with exception: " << ex.what() << "\n";
			}
			return 255;
			
		} else if (!strcmp(argv[i], "-c")){
			cflag = 1;
		} else if (!strcmp(argv[i], "-l")){
			lflag = 1;
		} else{
			std::cerr <<"Invalid flags\n";
			return 1;
		}

	}
	if (!strcmp(argv[i], "-c")){
			cflag = 1;
			i++;
	}

	if ( i+(2-cflag) != argc ) {
		std::cerr << "missing command line arguments\n";
		return 255;
	}
	try {
		std::ifstream input(argv[i], std::ifstream::binary);
		if (cflag){
			modelA<int, 16, 14> cmodel;

			compress(input, std::cout, cmodel);
			input.close();
		} else {
			i++;
			std::ofstream output(argv[i], std::ofstream::binary);
			modelA<int, 16, 14> cmodel;

			std::cout << "compressing...\n";
			compress(input, output, cmodel);
			input.close();
			output.close();
			if (lflag){
				int uncompressed = getFileSize(argv[i-1]);
				int compressed = getFileSize(argv[i]);
				std::cout<<"compressed size: " << compressed <<std::endl;
				std::cout<<"uncompressed size: " << uncompressed <<std::endl;
				std::cout<<"ratio: "<< std::fixed << std::setprecision(3)<<((float)(compressed)
					/uncompressed) * 100<<"%"<<std::endl;
			}
		}	
		return 0;
	}
	catch (std::exception &ex){
		std::cerr << "Failed with exception: " << ex.what() << "\n";
	}
	return 255;
}
