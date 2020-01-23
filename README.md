# Cardiac_Segmentation
Executables for segmenting left ventricle (LV) in cardiac CT images. (ver.1)<br/>


## Environments
caffe <br/>
<br/>
<br/>

## Trained weights
FCN-all-at-once-VGG16 was proposed as a training model for LV 2D segmentation, and the scheme of the model is shown below. Each trained weights at displacements 0, 1, and 2 are in the [*weights*](./weights) directory.<br/>
<img src="https://user-images.githubusercontent.com/17020746/72951565-32a01580-3dd2-11ea-9011-598a490f30a4.png" width="100%">
[*weights/displ0/snapshot_iter_54270.caffemodel*](./weights/displ0)<br/>
[*weights/displ1/snapshot_iter_54270.caffemodel*](./weights/displ1)<br/>
[*weights/displ2/snapshot_iter_54270.caffemodel*](./weights/displ2)<br/>
<br/>
<br/>

## LV Segmentation
#### Preparation of Data
```DOS.bat
cmd /c Preparation.exe .data/sample1.mha .data/sample1_displ1.mha 1 0 350
```
Preparation.exe는 12bit gray scale의 volume 영상을 8bit의 RGB로 이루어진 volume영상으로 rescale해주는 실행파일이다. 
입력값은 입력파일명, 출력파일명, displacement, (+옵션: window level, window width) 를 주면 된다. 아래의 코드는 data 디렉토리에 있는 sample1.mha 영상을 window level 0 HU, window width 350 HU로 windowing하고 각 slice 와 전, 후 slice정보(displacement=1)를 RGB로 merge하여 sample1_displ1.mha 영상으로 출력하는 명령어이다.
<br/>
<br/>
#### Segmentation with trained weights
```DOS.bat
cmd /c Preparation.exe .data/sample1.mha .data/sample1_disp1.mha 1 0 350
```
#### Label fusion with Majority Voting
```DOS.bat
cmd /c MajorityVoting.exe .data/mask1_displ0.mha .data/mask1_displ1.mha .data/mask1_displ2.mha .data/mask1_final.mha
```
