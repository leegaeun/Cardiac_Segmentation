# Cardiac_Segmentation
Executables for segmenting left ventricle (LV) in cardiac CT images. (ver.1)<br/>


## Environments
Windows 8.1 (64bit), Python 2.7.12, Anaconda 4.1.1, CUDA 7.5, cuDNN 4.0.7, [caffe-windows](https://github.com/BVLC/caffe/tree/windows) <br/>
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
#### [Step1] Preparation of Data
Preparation.exe는 12bit gray scale의 volume 영상을 8bit의 RGB로 이루어진 volume영상으로 rescale해주는 실행파일이다. 
입력값은 입력파일경로, 출력파일경로, displacement, (+옵션: window level, window width) 를 주면 된다. 아래의 코드는 data\sample1.mha 영상을 window level 150 HU, window width 200 HU로 windowing하고 각 slice 와 전, 후 slice정보(displacement=1)를 RGB로 merge하여 data\sample1_dis1.mha 영상으로 출력하는 명령어 예시이다.
```DOS.bat
cmd /c Preparation.exe data\sample1.mha data\sample1_dis1.mha 1 150 200
```
<br/>
<br/>
#### [Step2] Segmentation with trained weights
각 weights경로에는 CardiacSegmentation_python_dis*.py가 있는데, trained weights로 LV segmentation을 수행한다. 입력값은 step1에서 전처리과정을 끝낸 영상경로, 출력파일경로, weights가 있는 디렉토리경로이다.
아래의 코드는 data\sample1_dis1.mha 영상을 입력받아 segmented LV mask 파일을 출력하는 명령어 예시이다.
```DOS.bat
cmd /c python "%cd%\weights\displ1\CardiacSegmentation_python_dis1.py" "%cd%\data\sample1_dis1.mha" "%cd%\data\mask1_dis1.mha" "%cd%\weights\displ1"
```
#### [Step3] Label fusion with Majority Voting
MajorityVoting.exe는 step2에서 출력된 각 segmented mask를 Majoirity Voting으로 합쳐 최종 mask를 출력한다. 입력값은 합칠 mask파일경로들과 출력mask경로이다.
아래의 코드는 displacement=0,1,2에서의 LV mask인 data\mask1_dis0.mha, data\mask1_dis1.mha, data\mask1_dis2.mha 영상을 입력받아 data\mask1_final.mha 파일을 출력하는 명령어 예시이다.
```DOS.bat
cmd /c MajorityVoting.exe data\mask1_dis0.mha data\mask1_dis1.mha data\mask1_dis2.mha data\mask1_final.mha
```
<br/>
<br/>
<br/>
We also prepared two [sample dataset](./data) so that you can try the segmentation yourself. All of this data is unused for training.<br/>
