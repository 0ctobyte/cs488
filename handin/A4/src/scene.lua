-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)

scene = gr.node('root')

s3 = gr.nh_sphere('s3', {90, -40, -300}, 50)
scene:add_child(s3)
s3:set_material(mat3)

b2 = gr.nh_box('b2', {60.0, 70.0, -300}, 50)
scene:add_child(b2)
b2:set_material(mat4)

b1 = gr.nh_box('b1', {50, -125, -400}, 50)
scene:add_child(b1)
b1:set_material(mat4)

s1 = gr.nh_sphere('s1', {-90, 0, -400}, 50)
scene:add_child(s1)
s1:set_material(mat1)

s2 = gr.nh_sphere('s2', {-20, -100, -400}, 50)
scene:add_child(s2)
s2:set_material(mat2)

s = gr.sphere('s')
scene:add_child(s)
s:set_material(mat1)
s:scale(30.0, 40.0, 30.0)
s:translate(40.0, 50.0, -400.0)

scale = 30.0
t = scale*((1.0 + math.sqrt(5.0)) / 2.0);
i = scale*1.0
icosa = gr.mesh( 'icosahedron', {
	{-i, t, 0.0},
	{i, t, 0.0},
	{-i, -t, 0.0},
	{i, -t, 0.0},
	{0.0, -i, t},
	{0.0, i, t},
	{0.0, -i, -t},
	{0.0, i, -t},
	{t, 0.0, -i},
	{t, 0.0, i},
	{-t, 0.0, -i},
	{-t, 0.0, i},
}, {
  {0, 11, 5},
  {0, 5, 1},
  {0, 1, 7},
  {0, 7, 10},
  {0, 10, 11},
  {1, 5, 9},
  {5, 11, 4},
  {11, 10, 2},
  {10, 7, 6},
  {7, 1, 8},
  {3, 9, 4},
  {3, 4, 2},
  {3, 2, 6},
  {3, 6, 8},
  {3, 8, 9},
  {4, 9, 5},
  {2, 4, 11},
  {6, 2, 10},
  {8, 6, 7},
  {9, 8, 1}
})
--scene:add_child(icosa)
icosa:set_material(mat4)

white_light_radius = 1000.0
orange_light_radius = 5000.0

white_light = gr.light({-800.0, 500.0, -400.0}, {0.9, 0.9, 0.9}, {1.0, 2.0/white_light_radius, 1.0/(white_light_radius*white_light_radius)})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1.0, 2.0/orange_light_radius, 1.0/(orange_light_radius*orange_light_radius)})

gr.render(scene, 'scene.png', 1024, 1024,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
