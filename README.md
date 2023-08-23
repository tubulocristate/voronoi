Dynamic implemetation of voronoi diagram on GPU using OpenGL (voronoi_opengl) and static implemetation (votonoi_ppm). Elastic collision is benween seeds is implemented. To change distance metrics in OpenGl version, uncomment corresponding line in fragment shader

# Voronoi diagram with euclidean distance $$d(a, b) =\sqrt{\sum_{i=1}^n (a_i - b_i)^2}$$
![alt text](https://github.com/tubulocristate/voronoi/blob/main/euclidean.jpg?raw=true)


# Voronoi diagram with manhattan distance $$d(a, b) =\sqrt{\sum_{i=1}^n |a_i - b_i|}$$
![alt text](https://github.com/tubulocristate/voronoi/blob/main/manhattan.jpg?raw=true)


# Voronoi diagram with chebyshev distance $$d(a, b) =\underset{i}{\max}(|a_i - b_i|)$$
![alt text](https://github.com/tubulocristate/voronoi/blob/main/chebyshev.jpg?raw=true)

