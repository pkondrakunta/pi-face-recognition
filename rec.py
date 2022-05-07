import cv2
import pickle
import os
import face_recognition
import datetime
from imutils import paths

def recogniseFace(path):
    
    fc = cv2.CascadeClassifier("./cascades/haarcascade_frontalface_alt2.xml")

    data = pickle.loads(open('face_enc', "rb").read())
    image = cv2.imread(path)
    rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    
#     gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    faces = fc.detectMultiScale(rgb, scaleFactor=1.1, minNeighbors=6, minSize=(200, 200), flags=cv2.CASCADE_SCALE_IMAGE)

    encodings = face_recognition.face_encodings(rgb)
    
    names = []
    matches = []
    
    if(len(faces)>1):
        print("Multiple faces detected. Ensure that only one person is standing in front of the door to authenticate.")
        writeToFile("False","Multiple faces","None", "None")
	return 0

    for encoding in encodings:
        matches = face_recognition.compare_faces(data["encodings"],encoding)
        name = "Unknown"

    if True in matches:
        matchedIdxs = [i for (i, b) in enumerate(matches) if b]
        name = data['names'][0]

    if name == "Unknown":
        print(str(len(faces)) + " face(s) detected. Unable to recognise")
        writeToFile("False",name,"None","None")
	return 0
    else:
        print("Face(s) detected. Recognised as", name)
        uuid = datetime.datetime.now().strftime('%y%m%d_%H%M%S')
        path = "./recognised_images/" + uuid + ".jpg"
        writeToFile("True",name, uuid, path)
        cv2.imwrite(path, image)
        return 1

def writeToFile(name, uuid, path):
    textfile = open("result.txt", "w")
    a = textfile.write(name+","+uuid+","+path)
    textfile.close()
    print("Written to file!")
    
def backToMain():
    if recogniseFace("./testing_images/Image0.jpg") == 1:
        return True
    else:
        return False
    
