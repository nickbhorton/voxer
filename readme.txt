I found some random open source voxel format used for some decade old game.
All this program does is parse this file format and renders the voxels in the
least efficient way. All art assets are taken from said open source project (I
am not an artist)

Usage:
./render                  # will render teapot example
./render res/butcher.vox  # renders a cute character butcher
./render res/dragon.vox   # renders the classic dragon 
./render res/monu1.vox    # renders a cool building? 

Links:
https://github.com/ephtracy/voxel-model/ -> the voxel file format and art assets

Todo:
Better lighting and rendering abstraction -> imgui probably
    By rendering abstraction I mean a better interface to switch between
    optimization levels.
The real meat of the project: optimization progression implementations
    Somewhere between 5 and 10 different optimization levels -> imgui to
    switch between maybe.
Some summary document of what I found with the voxel optimizations
