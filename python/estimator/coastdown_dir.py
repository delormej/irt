import sys
import fnmatch
import os
import shutil

def find_calib_files(dir):
	matches = []
	for root, dirnames, filenames in os.walk(dir):
	  for filename in fnmatch.filter(filenames, 'calib_1.4*.csv'):
		  matches.append(os.path.join(root, filename))

	return matches

def main():
	temp = '/temp/coastdown/'
	files = find_calib_files('C:\\Users\\Jason\\SkyDrive\\InsideRide\\Tech\\Ride Logs\\')

	# copy files
	for idx, val in enumerate(files):
		shutil.copy2(val, temp)

	# process copies.
	copies = find_calib_files(temp)
	for idx, val in enumerate(copies):
		print("Processing", files[idx])
		os.system('coastdown.py ' + val)

if __name__ == "__main__":
	main()
