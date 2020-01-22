import caffe
import numpy as np
import SimpleITK as sitk

import os
import sys

input_file_name = sys.argv[1]
output_file_name = sys.argv[2]
curr_path = sys.argv[3]

MODEL_FILE = os.path.join(curr_path, "deploy.prototxt")
PRETRAINED = os.path.join(curr_path, "snapshot_iter_54270.caffemodel")

net = caffe.Net(MODEL_FILE, PRETRAINED, caffe.TEST)
# caffe.set_mode_cpu()
caffe.set_mode_gpu()
caffe.set_device(0)

image = sitk.ReadImage(input_file_name)
num_of_slices = image.GetDepth()
nda_3d = sitk.GetArrayFromImage(image)

out_img = sitk.Image(image.GetSize(), sitk.sitkUInt8)
out_img.SetDirection(image.GetDirection())
out_img.SetSpacing(image.GetSpacing())
out_img.SetOrigin(image.GetOrigin())

nda_3d_out = sitk.GetArrayFromImage(out_img)

for i in xrange(num_of_slices):
    nda_img = nda_3d[i,:,:]

    nda = nda_img.transpose(2,0,1)

    out = net.forward_all(data=np.asarray([nda]))

    nda_out = out['score']
    nda_out = np.argmax(nda_out, axis=1)
    nda_out = nda_out.astype(dtype=np.uint8)

    nda_3d_out[i,:,:] = nda_out.reshape(512,512)

out_img = sitk.GetImageFromArray(nda_3d_out)
sitk.WriteImage(out_img, output_file_name)
    
    
    
  
