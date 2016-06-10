### Author : Bhavani Chillapalli
### Date Created :04/24/2014
### NYU ID : N18909019
### Submission Date : 04/26/2014

import time
import os
import sys
import os.path
import json
import re
import math

#Device Id Checker:
#This checks if the Device Id in the super block is equal to 20.
def DeviceIdCheck():
	file_path ='C:/Users/Bhavani/Downloads/FS/fusedata.0'
	f=open(file_path,"r")
	for line in f:
		if "devId:20" in line:
			print ("Device Id is correct")
		else:
			print ("Device Id is incorrect")
#in the file system given, the device id is correct

DeviceIdCheck()


#TimeChecker:
#The time checker verifies that all the times are in past and none are in the future.It prints in an error if the former case is true.We verify this by comparing it with the current time
def TimeCheck():
	files = [0,26,27,30]
	now=time.time()
	file_path ='C:/Users/Bhavani/Downloads/FS/fusedata.'
	for i in files:
		f=open(file_path+str(i))
		dictionary = dict()
		for line in f:
			listOfWords = line.split(', ')
			for word in listOfWords:
				line=re.sub('{ }', ' ', word)#remove only {}
				#print('Entering for loop')
				splitWithCol = line.split(':')
				dictionary[splitWithCol[0].replace("{","")] = splitWithCol[1]
		if dictionary.get("creationTime","empty")!="empty":
			if int(dictionary['creationTime'])<now:
				print ("Creation Time is Correct!")
			else:
				print ("Creation Time is Incorrect!")
		if dictionary.get("atime","empty")!="empty":
			if int(dictionary['atime'])<now:
				print ("atime attribute is Correct!")
			else:
				print ("atime attribute is incorrect!")
		if dictionary.get("ctime","empty")!="empty":
			if int(dictionary['ctime'])<now:
				print ("ctime attribute is Correct!")
			else:
				print ("ctime attribute is incorrect!")
		if dictionary.get("mtime","empty")!="empty":
			if int(dictionary['mtime'])<now:
				print ("mtime attribute is Correct!")
			else:
				print ("mtime attribute is incorrect!")
# in the given file system,the mtime in fusedata.26 and fusedata.30 are incorrect.
TimeCheck()
#FreeblockChecker
#these verify that the empty files are printed in the blocks 1 to 27. if the size of a file is 0, we have assumed that ti is empty and hasnt been written to.
def fb_check():
	file_path ='C:/Users/Bhavani/Downloads/FS/fusedata.'
	for i in range(27,10000):
		f=file_path+str(i)
		if os.path.exists(f) == True:
			size=os.path.getsize(f)
			if size==0:
				index = math.floor(i/400) + 1
				fo = open(file_path+str(index),"r")
				if " "+str(i)+"," not in fo.read():
					print("Error!FreeBlockList Incorrect!")

def fb_check2():
	file_path ='C:/Users/Bhavani/Downloads/FS/fusedata.'
	for i in range(1,27):
		f=file_path+str(i)
		fo = open(f,"r")
		for line in fo:
			fileNumber = line.strip().split(",")
			for j in fileNumber:
				emptyFile=file_path+str(j)	
				if os.path.exists(emptyFile) == True:
					size=os.path.getsize(emptyFile)
					if size!=0:
						print("ERROR!!")
				else:
					 print("File does not exist!")
# in the given file system, there are no errors produced.The free block list is correct. 
	
		
fb_check()
fb_check2()

def link_check():
	file_path ='C:/Users/Bhavani/Downloads/FS/fusedata.'
	for i in range(26,28):
		f=open(file_path+str(i),"r")
		for line in f:
			tokens= line.strip().split("{")
			if len(tokens) > 2:
				dictionary = dict()
				listOfWords = tokens[1].strip().split(',')
				for word in listOfWords:
					keyValuArray = word.split(":")
					dictionary[keyValuArray[0].strip()] = keyValuArray[1]
					#print('Entering for loop')
				inodeList = tokens[2].strip().split(',')
				print(inodeList)
				print(dictionary)
				if dictionary.get("linkcount","empty")!="empty":
					if len(inodeList) != int(dictionary["linkcount"]):
						print("Link count is not matching with inode dictionary"+"fusedata"+str(i))
					else:
						print("link count matches")
			
link_check()

def indirect_check():
  file_path = 'C:/Users/Bhavani/Downloads/FS/fusedata.'
  for i in range(26,31):
        f = file_path + str(i)
        fo = open(f,"r")
        for line in fo:
           listOfWords=line.strip().split(",")
         #print ("listOfWords")
        dictionary = dict()
        for each in listOfWords:
          words = each.split(":")
          if(len(words)>1):
            keyarray = re.sub('[]{}', '', words[0])
            valuearray = re.sub('[]{}', '', words[1])
            dictionary[keyarray] = valuearray
        #print(dictionary)
        if (dictionary.get("filename_to_inode_dict","empty")!="empty"):
          print ("Empty!")
        if (dictionary.get("size","empty")!="empty"):
           if(int(dictionary['size']) <= 4096) and  (dictionary.get("indirect","empty")!="empty"):
                 if(int(dictionary['indirect'][0])!= 0):
                    print("Indirect number is incorrect as it is 1")
                 else:
                 	print("xyz")
           else:
            if (dictionary.get("indirect","empty")!="empty") and (int(dictionary['indirect'][0])!=1):
                    print("Indirect number is incorrect as it is 1")
            else:
              print(" Indirect number does not exist") 

indirect_check()



def indirect_check2():
	file_path = 'C:/Users/Bhavani/Downloads/FS/fusedata.27'
	fo = open(file_path,"r")
	print (fo)
	for line in fo:
	    listOfWords=line.strip().split(",")
	    dictionary = dict()
	    for each in listOfWords:
	        words = each.split(":")
	        if(len(words)>1):
	            keyarray = re.sub('[ ]{}', '', words[0])
	            valuearray = re.sub('[ ]{}', '', words[1])
	            dictionary[keyarray] = valuearray
	            print (dictionary)
	        '''    
		    string= str(dictionary['location'])
		    fp=open(file_path+string,"r")
		    f=f.read(1)
		    if directory['indirect']==1 :
				if f!='{' :	
					print('Indirect is correct!')
				else:
					print('Indirect is wrong!')
			
			if dictionary['indirect']==0 :
				if f!='{' :	
					print('Indirect is correct!')
				else:
					print('Indirect is wrong!')
				
indirect_check2()
'''
			




