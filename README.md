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
cmd /c Preparation.exe ./input.mha ./output_disp0.mha 0
```
Preparation.exe는 12bit gray scale의 volume 영상을 8bit의 RGB로 이루어진 volume영상으로 rescale해주는 실행파일이다. 
입력값은 입력파일명, 출력파일명, displacement를 주면 된다. 아래의 코드는 samples 디렉토리에 있는 sample1.mha파일을 
Codes for detecting hepatic malignancy with the trained weights are implemented in [*Run_test.ipynb*](./Run_test.ipynb).<br/>
<br/>
<p>
<img src="https://user-images.githubusercontent.com/17020746/68740110-a92fd280-062d-11ea-813f-2659629ae564.png" width="30%">    <img src="https://user-images.githubusercontent.com/17020746/68740198-e09e7f00-062d-11ea-9320-e14bfa929da0.png" width="30%">    <img src="https://user-images.githubusercontent.com/17020746/68740192-de3c2500-062d-11ea-97f3-a9d7d7aca680.png" width="30%">
</p>
When the above multiphase CT scan (arterial-, portal venous-, delayed phase from left) is input,<br/>
malignancy is detected as shown below. White is ground-truth and Red is the predicted detection box.<br/>
<br/>

<p>
<img src="https://user-images.githubusercontent.com/17020746/68740581-bc8f6d80-062e-11ea-97dd-16685ad219bf.png" width="50%">
</p>

<br/>


We also prepared [test dataset](./data) so that you can try the detection yourself. All of this data is unused for training and includes hepatic malignancies.<br/>
Just run after modifying the following two lines in [*Run_test.ipynb*](./Run_test.ipynb) to suit your input path.<br/>
<pre>imgPath = "./data/img"
maskPath = "./data/label_gt"<code>
<br/>
