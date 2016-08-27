import sys
import os

def deploy(hex_path):	
	# grab the path of the nrfutil and add it to the environment
	nrfutil_path = os.path.join(sys.exec_prefix, 'Scripts')
	sys.path.append(nrfutil_path)

	# archive name is the same, except ends in .zip
	zip_path = hex_path.replace('.hex', '.zip')

	# using os.system because script has a '-' in the name which makes execfile/exec impossible
	# I also cannot import the module?? 
	# note, this will just bomb, so much for try/catch below - it won't catch error
	os.system("nrfutil dfu genpkg --application " + hex_path + " " + " " + zip_path)  

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
