import csv

class fileReader:
	def __init__ (self, data):
		self.data = data
	def getData (self):
		with open (self.data,mode='r') as infile:
			reader = csv.reader(infile)
			Nish = {}
			for rows in reader:
				Nish[(rows[0],rows[1])] = rows[2] 
			return Nish


