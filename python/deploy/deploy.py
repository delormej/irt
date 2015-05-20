import sys
import zipfile as zip
import os

def deploy(hex_path):
	# archive name is the same, except ends in .zip
	zip_path = hex_path.replace('.hex', '.zip')
	
	with zip.ZipFile(zip_path, 'w') as zipPkg:
		zipPkg.write(hex_path, os.path.basename(hex_path))
		zipPkg.write('application.dat')
		
	return zip_path

		
if __name__ == "__main__":
	if len(sys.argv) < 2:
		print("ERROR! No path to hex specified\nusage: deploy.py <path_to_hex>")
		sys.exit(2)
	hex_path = sys.argv[1]

	try:
		zip_path = deploy(hex_path)
		print("SUCCESS! Created archive at %s." % zip_path)
	except:
		print("ERROR unable to create zip archive.")		
