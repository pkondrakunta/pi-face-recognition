from imutils import paths
import face_recognition
import pickle
import cv2
import os

#Folder 'training_images' contains data( individual image folders of various people)
imagePath = list(paths.list_images('training_images'))
kEncodings = []
kNames = []

# loop over the image paths

for (i, ip) in enumerate(imagePath):
    # extract the person name from the image path
    name = ip.split(os.path.sep)[-2]
    # load the input image and convert it from BGR
    image = cv2.imread(ip)
    rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    boxes = face_recognition.face_locations(rgb,model='hog')
    # compute the facial embedding for the any face
    encodings = face_recognition.face_encodings(rgb, boxes)
    
for encoding in encodings:
    kEncodings.append(encoding)
    kNames.append(name)
    
    #save emcodings along with their names in dictionary data
    data = {"encodings": kEncodings, "names": kNames}
    
    #saving to a pickle to use it later
    f = open("face_enc", "wb")
    f.write(pickle.dumps(data))
    f.close()
