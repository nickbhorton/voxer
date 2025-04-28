scene:
    3,688,048 voxels

render1:
    6.75 fps over 90 frames
    6.63 fps over 1002 frames

    44,256,576 triangles
    132,769,728 vertexes
    at 6 floats per vertex
    at 4 bytes per float
        2.96763 Gb of geometry data

    1 draw call per frame

render2: chunking at 256
    6.61 fps over 169 frames

    44,256,576 triangles
    132,769,728 vertexes
    at 6 floats per vertex
    at 4 bytes per float
        2.96763 Gb of geometry data

    16 draw call per frame

render3: uint instead of vec3 for pos and col
    6.61 fps over 157 frames

    44,256,576 triangles
    132,769,728 vertexes
    at 2 uint per vertex
    at 4 bytes per uint
        0.989212 Gb of geometry data

    16 draw call per frame

render4: use ssbo for color array and put color index into position uint
    6.71 fps over 97 frames

    44,256,576 triangles
    132,769,728 vertexes
    at 1 uint per vertex
    at 4 bytes per uint
        0.494606 Gb of geometry data

    16 draw call per frame


render5: do per face instance rendering
    8.76 fps over 152 frames

    2 triangles
    4 vertexes
    at 3 float per vertex
    at 4 bytes per float
         48 of geometry data

    22,128,288 instances calls of model
    3,688,048 unique data with 6 divisor
    4 byte per unique data
    ~14 mega bytes of unique data

    16 draw call per frame

render6: eliminate covered triangles
    43.7 fps over 1005 frames

    2 triangles
    4 vertexes
    at 3 float per vertex
    at 4 bytes per float
         48 of geometry data
    
    faces per direction per model (16 models)
    59120 -> down 
    59120 -> up 
    26550 -> north
    39743 -> east
    26550 -> south
    39743 -> west
    total = 250,826 faces (quads) per model
          = 501,652 triangles per model
          = 8,026,432 triangles total

    16 draw calls per frame
