
C_SRC  := ~/code
BINDIR := ${C_SRC}/bin

SCORE_PY := Measure.py

DOWNSCALED_PATH :=/downscaled
UPSCALED_PATH := ~/upscaled
FPGA_OUT_PATH := ~/fpga_out

SRC_BMP := 0.bmp
TARGET_BMP := 0.bmp

SRC_BMP_WIDTH := 960
SRC_BMP_HEIGHT := 540

TARGET_BMP_WIDTH := 3840
TARGET_BMP_HEIGHT := 2160

THRESHOLD := 144

C_FILE := $(shell find $(C_SRC)/ -name "*.c" -or -name "*.h")

bin: ${BINDIR}/fast_nedi_cmodel ${BINDIR}/downsampling ${BINDIR}/compare_bmp ${BINDIR}/read_header ${BINDIR}/scale.o ${BINDIR}/bmp.o

${BINDIR}/fast_nedi_cmodel ${BINDIR}/downsampling ${BINDIR}/compare_bmp ${BINDIR}/read_header ${BINDIR}/scale.o ${BINDIR}/bmp.o : ${C_FILE}
	mkdir -p ${BINDIR}
	gcc -c ${C_SRC}/bmp.c -std=c99 -Wall -lm -o ${BINDIR}/bmp.o 
	gcc -c ${C_SRC}/scale.c -std=c99 -Wall -lm -o ${BINDIR}/scale.o 
	gcc ${C_SRC}/bmp_downsampling_main.c ${BINDIR}/scale.o ${BINDIR}/bmp.o -std=c99 -Wall -lm -o ${BINDIR}/downsampling 
	gcc ${C_SRC}/read_header_main.c ${BINDIR}/bmp.o -std=c99 -Wall -lm -o ${BINDIR}/read_header
	gcc ${C_SRC}/bmp_upsampling_main.c ${BINDIR}/scale.o ${BINDIR}/bmp.o -std=c99 -Wall -lm -o ${BINDIR}/fast_nedi_cmodel
	gcc ${C_SRC}/compare_bmp_main.c ${BINDIR}/bmp.o -std=c99 -Wall -lm -o ${BINDIR}/compare_bmp


run: bin
	cd ${BINDIR} && fast_nedi_cmodel ${DOWNSCALED_PATH}/${SRC_BMP} ${TARGET_BMP_WIDTH} ${TARGET_BMP_HEIGHT} ${THRESHOLD} ${UPSCALED_PATH}/${SRC_BMP}

cmp: bin
	cd ${BINDIR} && compare_bmp  ${UPSCALED_PATH}/${SRC_BMP}  ${FPGA_OUT_PATH}/${TARGET_BMP} 
score:
	cd ${C_SRC} && python3 ${SCORE_PY}
clean:
	rm -rf  ${UPSCALED_PATH}/${SRC_BMP}
	rm -rf  ${BINDIR}/
