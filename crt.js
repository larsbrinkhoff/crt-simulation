/*	Enabling strict javascript means fewer silent errors.
*/

'use strict';

function Radians(angle_in_degrees) 
{
	return angle_in_degrees * (Math.PI / 180);
}

function Degrees(angle_in_radians)
{
	return angle_in_radians / Math.PI  * 180;
}

function setup_shader(id)
{
	let vrtx = document.getElementById(vrtx_id);
	if (!vrtx)
		throw "Could not find script element " + vrtx_id;
	vrtx = vrtx.text;

	let shader = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(shader, vrtx);
	gl.compileShader(shader);
	success = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
	if (!success)
		throw "Could not compile vertex shader:" + gl.getShaderInfoLog(shader);

	let program = gl.createProgram();
	gl.attachShader(program, shader); 
	gl.linkProgram(program);
	success = gl.getProgramParameter(program, gl.LINK_STATUS);
	if (!success)
		throw ("Shader program filed to link:" + gl.getProgramInfoLog (program));
		
	return program;
}

function CreateShader(vrtx_id, frag_id)
{
	if (!vrtx_id)
		throw "Parameter 1 to CreateShader may be missing.";

	if (!frag_id)
		throw "Parameter 2 to CreateShader may be missing.";

	let success;

	let vrtx = document.getElementById(vrtx_id);
	if (!vrtx)
		throw "Could not find script element " + vrtx_id;
	vrtx = vrtx.text;

	let frag = document.getElementById(frag_id);
	if (!frag)
		throw "Could not find script element " + frag_id;
	frag = frag.text;

	let vertShader = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertShader, vrtx);
	gl.compileShader(vertShader);
	success = gl.getShaderParameter(vertShader, gl.COMPILE_STATUS);
	if (!success)
		throw "Could not compile vertex shader:" + gl.getShaderInfoLog(vertShader);

	let fragShader = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragShader, frag);
	gl.compileShader(fragShader);
	success = gl.getShaderParameter(fragShader, gl.COMPILE_STATUS);
	if (!success)
		throw "Could not compile fragment shader:" + gl.getShaderInfoLog(fragShader);

	let shaderProgram = gl.createProgram();
	gl.attachShader(shaderProgram, vertShader); 
	gl.attachShader(shaderProgram, fragShader);
	gl.linkProgram(shaderProgram);
	success = gl.getProgramParameter(shaderProgram, gl.LINK_STATUS);
	if (!success)
		throw ("Shader program filed to link:" + gl.getProgramInfoLog (shaderProgram));
		
	return shaderProgram;
}

function InitializeShader() {
	solid_shader.program = CreateShader("vertex_shader", "fragment_shader");
	console.log('Program: ' + solid_shader.program);
	gl.useProgram(solid_shader.program);
	solid_shader.a_vertex_coordinates = gl.getAttribLocation(solid_shader.program, "a_vertex_coordinates");
	solid_shader.a_colors = gl.getAttribLocation(solid_shader.program, "a_colors");
	solid_shader.u_mv = gl.getUniformLocation(solid_shader.program, "u_mv");
	solid_shader.u_pj = gl.getUniformLocation(solid_shader.program, "u_pj");
	gl.useProgram(null);
	console.log('Vertex Coordinate handle: ' + solid_shader.a_vertex_coordinates);
	console.log('Color attribute handle: ' + solid_shader.a_colors);
	console.log('MV Matrix handle: ' + solid_shader.u_mv);
	console.log('PJ Matrix handle: ' + solid_shader.u_pj);
}

function InitializeTriangle()
{
	triangle.vao = gl.createVertexArray();
	triangle.vrts_buffer = gl.createBuffer();
	triangle.indx_buffer = gl.createBuffer();
	triangle.colr_buffer = gl.createBuffer();

	triangle.vrts = [ ];
	let p = vec3.create();
	let m = mat4.create();
	triangle.vrts.push(x_axis[0], x_axis[1], x_axis[2]);
	for (let i = 0; i < 2; i++) {
		mat4.rotate(m, m, Radians(120), z_axis);
		vec3.transformMat4(p, x_axis, m);
		triangle.vrts.push(p[0], p[1], p[2]);
	}
	console.log(triangle.vrts);
	triangle.colors = [ 1, 0, 0, 0, 0, 1, 0, 1, 0 ];
	triangle.indices = [ 0, 1, 2 ];

	gl.bindVertexArray(triangle.vao);
	gl.bindBuffer(gl.ARRAY_BUFFER, triangle.vrts_buffer);
	gl.vertexAttribPointer(solid_shader.a_vertex_coordinates, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(solid_shader.a_vertex_coordinates);
	gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(triangle.vrts), gl.STATIC_DRAW);

	gl.bindBuffer(gl.ARRAY_BUFFER, triangle.colr_buffer);
	gl.vertexAttribPointer(solid_shader.a_colors, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(solid_shader.a_colors);
	gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(triangle.colors), gl.STATIC_DRAW);

	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, triangle.indx_buffer);
	gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(triangle.indices), gl.STATIC_DRAW);

	/*	This unbinding of the VAO is necessary.
	*/

	gl.bindVertexArray(null);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);

	console.log('Vertex buffer: ' + triangle.vrts_buffer);
	console.log('Vertices: ' + triangle.vrts);
	console.log('VAO: ' + triangle.vao);
}

function ProjectText(P, mvp, ctx, text)
{
	let p = vec4.clone(P);
	vec4.transformMat4(p, p, mvp);
	p[0] /= p[3];
	p[1] /= p[3];
	let c = vec2.fromValues((p[0] * 0.5 + 0.5) * gl.canvas.width, (p[1] * -0.5 + 0.5) * gl.canvas.height);
	ctx.fillText(text, c[0], c[1]);
}

function DrawScene(now) {
	/*	Convert milliseconds to seconds.
	*/

	now /= 1000;

	/*	Place title.
	*/
	
	ct.clearRect(0, 0, ct.canvas.width, ct.canvas.height);
	ct.font = "32px Helvetica";
	ct.fillStyle = "#404040";
	ct.fillText("Hello World", 20, 50);
	
	/*	Initialize gl drawing area.
	*/

	gl.clearColor(0.1, 0.1, 0.1, 1.0);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	gl.viewport(0, 0, glcanvas.clientWidth, glcanvas.clientHeight);

	let projection_matrix = mat4.create();
	let modelview_matrix = mat4.create();
	let mvp = mat4.create();
	let p = vec3.create();

	mat4.lookAt(modelview_matrix, vec3.fromValues(0.0, 0.0, 5.0), vec3.fromValues(0.0, 0.0, 0.0), y_axis);
	mat4.perspective(projection_matrix, Radians(30.0), glcanvas.clientWidth / glcanvas.clientHeight, near_plane, far_plane);
	mat4.rotate(modelview_matrix, modelview_matrix, now, z_axis);
	mat4.rotate(modelview_matrix, modelview_matrix, now, x_axis);
	
	gl.useProgram(solid_shader.program);
	gl.uniformMatrix4fv(solid_shader.u_mv, false, modelview_matrix);
	gl.uniformMatrix4fv(solid_shader.u_pj, false, projection_matrix);
	
	gl.bindVertexArray(triangle.vao);
	gl.drawElements(gl.TRIANGLES, 3, gl.UNSIGNED_SHORT, 0);
	gl.bindVertexArray(null);
	gl.useProgram(null);

	requestAnimationFrame(DrawScene);
}

var vec3 = glMatrix.vec3;
var mat4 = glMatrix.mat4;
var vec4 = glMatrix.vec4;
var vec2 = glMatrix.vec2;

var x_axis = vec3.fromValues(1, 0, 0);
var y_axis = vec3.fromValues(0, 1, 0);
var z_axis = vec3.fromValues(0, 0, 1);
var near_plane = 1;
var far_plane = 100;

var glcanvas = document.getElementById('glcanvas');
var txcanvas = document.getElementById("txcanvas");
var gl = glcanvas.getContext('webgl2');
var ct = txcanvas.getContext("2d");
var solid_shader = {};
var triangle = {};

ct.textAlign = "left";
ct.textBaseline = "bottom";
ct.fillStyle = "#ffffff";

setup_shader("phosphor");

InitializeShader();
InitializeTriangle();
requestAnimationFrame(DrawScene)
