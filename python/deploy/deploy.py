import sys
import zipfile as zip
import os

def deploy(bin_path):
	# archive name is the same, except ends in .zip
	zip_path = bin_path.replace('.bin', '.zip')
	appdat_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'application.dat')
	manifest_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'manifest.json')
	
	with zip.ZipFile(zip_path, 'w') as zipPkg:
		zipPkg.write(bin_path, 'application.bin')
		zipPkg.write(appdat_path, os.path.basename(appdat_path))
		zipPkg.write(manifest_path, os.path.basename(manifest_path))
		
	return zip_path

		
if __name__ == "__main__":
	if len(sys.argv) < 2:
		print("ERROR! No path to bin specified\nusage: deploy.py <path_to_bin>")
		sys.exit(2)
	bin_path = sys.argv[1]

	try:
		zip_path = deploy(bin_path)
		print("SUCCESS! Created archive at %s." % zip_path)
	except:
		print("ERROR unable to create zip archive.")		
