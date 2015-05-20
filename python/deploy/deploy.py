import sys
import zipfile as zip

def deploy(hex_path):
	# parse file name out:
	print(hex_path)
	name = 'IRT_xxxxxxx' # from hex_path
	with zip.ZipFile(name + '.zip', 'w') as zipPkg:
		zipPkg.write(hex_path, name + '.hex')
		zipPkg.write('application.dat')
		
		
if __name__ == "__main__":
	hex_path = sys.argv[1]
	deploy(hex_path)
