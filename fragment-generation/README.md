# How to generate random jigsaw fragments from a whole picture:

* put pictures to be cut into ```image``` folder
* (recommended) If you want to have fragments that approximate square, give two parameters, ```row``` and ```col```. For example, if you want to cut a picture into ```6 rows``` and ```10 columns```:

	```python3 main.py ./image 6 10```
	
* (very slow) If you want triangular or trapezoidal fragments, give one parameter: how many pieces do you want.

	For example, if you want to cut a picture into ```5 pieces```:

	```python3 main.py ./image 5```
	
* Fragments will appear in a folder with the same name as the picture. 
	
	**Use fragments in the ```filename/std``` folder**
	

