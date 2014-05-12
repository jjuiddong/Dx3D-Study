
global Outfile -- 저장될 file
global g_FilePath

global g_varSetMeshes = #()


fn ConvertToD3D_POS max_pos =
(
	local d3d_pos = [0,0,0]
	d3d_pos.x = max_pos.x
	d3d_pos.y = max_pos.z
	d3d_pos.z = max_pos.y
	
	d3d_pos
)


fn ConvertToD3D_INDEX max_index =
(
	local d3d_index = [0,0,0]
	d3d_index.x = max_index.x - 1
	d3d_index.y = max_index.z - 1
	d3d_index.z = max_index.y - 1

	d3d_index
)


fn CollectObject = 
(
	g_varSetMeshes = #()

	for i=1 to objects.count do
	(
		append g_varSetMeshes objects[i]
	)
)


fn CollectVertex obj = 
(
	varSetVertex= #()
	for i=1 to obj.numverts do
	(
		v = getvert obj i
		append varSetVertex v
	)
	varSetVertex
)

fn CollectIndex obj = 
(
	varSetIndex = #()
	for i=1 to obj.numfaces do
	(
		v = copy (getface obj i)
		append varSetIndex v
	)
	varSetIndex
)


fn WriteVertex obj = 
(
	varSetVertice = CollectVertex obj

	format "VERTEX = %\n" varSetVertice.count to:Outfile
	for i=1 to varSetVertice.count do
	(
		vtx = (ConvertToD3D_POS varSetVertice[ i]) 
		format "% % %\n" vtx.x vtx.y vtx.z to:Outfile
	)
)


fn WriteIndex obj = 
(
	varSetIndex = CollectIndex obj

	format "INDEX = %\n" varSetIndex.count to:Outfile
	for i=1 to varSetIndex.count do
	(
		index = (ConvertToD3D_INDEX varSetIndex[ i])
		format "% % %\n" (index.x as integer) (index.y as integer) (index.z as integer) to:Outfile
	)
)


fn ExporterStart = 
(
	CollectObject()

	Outfile = createfile g_FilePath

	for i=1 to g_varSetMeshes.count do
	(
		print g_varSetMeshes[i] 
		print g_varSetMeshes[i].name 
		
		-- vertex, index값은 EditMesh모드일때만 얻어올수 있어서 ConvertToMesh 함수를
		-- 사용했다. 문제는 EditMesh모드일때 VertexWeight값을 얻어올수 없다는것이다.
		-- 그래서 Vertex, index값을 얻은 후 undo명령어로 이전상태로 바꾼후 얻어온다.
		undo on
		(
			ConvertToMesh g_varSetMeshes[ i]
			WriteVertex g_varSetMeshes[ i]
			WriteIndex g_varSetMeshes[ i]
		)
		max undo
	)

	close Outfile

)



rollout ui "냠냠" width:387 height:266
(
	button btnRun "DX Exporter 실행" pos:[101,108] width:177 height:30

	-- Bmm, Ani 파일명 입력창
	label lbl1 "파일명:" pos:[30,13] width:70 height:17
	edittext editFilePath "" pos:[100,10] width:250 height:17 enabled:true

	-- UI 창이 열릴때 
	on ui open do
	(
		editFilePath.text = "data.txt"
	)

	-- DirectX Export
	on btnRun pressed do
	(
		g_FilePath = editFilePath.text
		ExporterStart()
	)

)

MainUI = newRolloutFloater "Simple Exporter" 400 300
addrollout ui MainUI

