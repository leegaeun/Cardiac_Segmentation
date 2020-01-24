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
*Preparation.exe* is an executable file that rescales 12-bit gray scale volume image to 8-bit RGB volume image. The inputs are 'input file path', 'output file path', 'displacement', and 'window level(HU)', 'window width(HU)' options. <br/>
The following code is an example of a command to output 'data\sample1_dis1.mha' image by merging each slice information (displacement = 1) with RGB after windowing 'data\sample1.mha' image at level 150HU and width 200HU.
```DOS.bat
cmd /c Preparation.exe data\sample1.mha data\sample1_dis1.mha 1 150 200
```

#### [Step2] Segmentation with trained weights
Each weights path has 'CardiacSegmentation_python_dis*.py', which performs LV segmentation with trained weights. The inputs are 'file path of the preprocessed image in step1', 'output file path', and 'directory path with weights'. <br/>
The following code is an example of a command to output segmented LV mask file('data\mask1_dis1.mha') by inputting 'data\sample1_dis1.mha' image.
```DOS.bat
cmd /c python "%cd%\weights\displ1\CardiacSegmentation_python_dis1.py" "%cd%\data\sample1_dis1.mha" "%cd%\data\mask1_dis1.mha" "%cd%\weights\displ1"
```

#### [Step3] Label fusion with Majority Voting
*MajorityVoting.exe* combines the segmented masks output in [step2] into Majority Voting and outputs the final mask. <br/>
The following code is an example of outputting a *data\mask1_final.mha* file by fusion of *data\mask1_dis0.mha*, *data\mask1_dis1.mha* and *data\mask1_dis2.mha* images, which are LV masks at displacement=0,1,2.
```DOS.bat
cmd /c MajorityVoting.exe data\mask1_dis0.mha data\mask1_dis1.mha data\mask1_dis2.mha data\mask1_final.mha
```
<br/>
<br/>

We also prepared two [sample dataset](./data) so that you can try the segmentation yourself. All of this data is unused for training.<br/>
