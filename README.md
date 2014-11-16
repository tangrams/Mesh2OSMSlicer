## Non traditional 3D Mapping
##### From pointClouds to GeoJSON Polygons

The last month I have been experimenting on different ways points cloud can be use for mapping big areas. The following is a brief documentation of my research that contains some of the tools and libraries, I found and make in my way. 

### Working with Point Clouds

PointClouds are not new, but are trending now. New devices with more computational power and memory are making this technology more common. For me (as other artist) this technology become accesible with the lunch of Microsoft Kinect camera. A cheap and easy to hack devices that use structured light to get depth information in form of stream of video. Making point clouds of it was easy and gratifying.

Since the arrival of the kinect new products where presented to the market; like Google’s [Tango Project](https://www.google.com/atap/projecttango/#project), [Leap Motion](https://www.leapmotion.com/) and [Structure](http://structure.io/). Around this devices some open source initiatives have develop libraries such as: [freenect](http://openkinect.org/wiki/Main_Page), [openNI](http://structure.io/openni) and [libPCL](http://pointclouds.org/).

#### LIDar data

![00](images/00.jpg)

Way before the Kinect came to the market, point clouds where use to map thanks to a technology develop on the earlier ’60 call Lidar (that stands for Light Detection and Ranging). It consist on lasers mounted under planes that send pulses of light to the surface of a terrain measuring the precise distance to it, then this information is geolocated by GPS, producing a set of millions of points perfectly geolocated in 3D space. There are a couple of programs that let you see and edit this big amount of data, the best open source application I found was [CloudCompare](http://www.danielgm.net/cc/).

![00](images/00a.jpg)

Once the data is loaded on a GeoSpacial database such us [PostGIS](http://postgis.net/) it can be use to cross information to any map. This is is case of [Yuriy Czoli’s tutorial](https://gist.github.com/YKCzoli/3605e014b8ed09a571e5) were he set the height of [OpenStreetMap](http://www.openstreetmap.org/) buildings.

In [Mapzen](www.mapzen.com) we use the [OpenStreetMap](http://www.openstreetmap.org/) height to generate 3D maps in [tangram](https://mapzen.com/tangram#mapzen,40.70531887544228,-74.00976419448854,16), my interest on Point Clouds was more focus on who to improve the models it self.

![00](images/00b.png)

Coming from an artistic approach and not knowing much about GIS, I decide to try my own way into this technology. For that I made a series of scripts and tools in this [repository](https://github.com/tangrams/LIDar-tools) to teach my self how to convert Lidar data to other formats, [export it to PostGIS](https://gist.github.com/patriciogonzalezvivo/229c5cd4001c2ed45ec6) and finally extract single buildings by query.

In the [Lidar-Tools repository](https://github.com/tangrams/LIDar-tools) you will find: 

* ```las2SM```: re-project lidar files (.laz/.las) to [Spherical Mercator (epsg:3857)](http://epsg.io/3857) 

* ```las2tile```: crop a tile from a lidar files (.laz/.las) and project to [Spherical Mercator (epsg:3857)](http://epsg.io/3857) 

* ```las2ply```: export lidar files (.laz/.las) into .ply format

* ```getPointsForID```: once a PostGIS database is loaded with lidar information of a region, this scritps get all the points inside a tagged OSM polygon by providing the OSM ID.

This tools together with the one provides by the others that [libLAS](http://www.liblas.org/) allows me to successfully tag a building, the Flatiron, and extract it with precession. 

Why the Flatiron?, well is a beautiful and perfectly isolated building that happens to be two blocks from where I work.

#### Making point-clouds from pictures

But “How owns a Lidar? or data Lidar data?”. Well they are expensive devices, only companies and institutions can afford even to rent one, that’s how far the “openness” of Lidar goes.

Buts there is a way around, a relative “old” technique call photogrammetry. It was original develop on 2006 under the name of PhotoTurism. What it does is to stich photographies together finding distinctive assets and cross similarities to construct 3D point clouds reconstructions.

![01](images/01.jpg)

Today the most most popular applications using this technology are [PhotoSynth](https://photosynth.net/) and [123D Catch](http://www.123dapp.com/catch). But the are some powerful open source equivalent such us [Bundler](http://www.cs.cornell.edu/~snavely/bundler/).

<iframe width="575" height="323" src="//www.youtube.com/embed/vpTEobpYoTg" frameborder="0" allowfullscreen></iframe>

“*Bundler takes a set of images, image features, and image matches as input, and produces a 3D reconstruction of camera and (sparse) scene geometry as output. The system reconstructs the scene incrementally, a few images at a time, using a modified version of the [Sparse Bundle Adjustment](http://users.ics.forth.gr/~lourakis/sba/) package of Lourakis and Argyros as the underlying optimization engine. Bundler has been successfully run on many Internet photo collections, as well as more structured collections*.”

I start taking pictures of the Flatiron together with Karim (a friend at work) and his reflect camera. Following [Jesse’s Tutorial](http://wedidstuff.heavyimage.com/index.php/2013/07/12/open-source-photogrammetry-workflow/) I learn how to take pictures that work best with the bundler algorithm, how to use [VisualSfM](http://ccwu.me/vsfm/) for the reconstructions and [MeshLab](http://meshlab.sourceforge.net/) for editing the point clouds.

![01](images/01a.gif)

So far so good, but there was a problem, this points clouds wasn’t geo-location at all, in fact are points floating in a random 3D universe. The data is now worth it for mapping if it can not be located in space and add to PostGIS. 

So I took more photos using my cellphone with the hope of using the GPS location wrote on the EXIF header of the pictures. For experimenting with it I made an [openFrameworks addon](https://github.com/patriciogonzalezvivo/ofxBundle) that let me import the bundle file and dense reconstruction, together with the GPS location, only to fin out that phones GPS data is incredible noisy. The effect is call [“urban canyon”](http://en.wikipedia.org/wiki/Street_canyon) and is produce by the satellites signals bouncing between buildings.

Just to proof my point I end up adjusting the difference by hand and successfully match their location with the rest of OSM data set. You can see how not just the Flat iron fits to the OSM building but also the rest of the near constructions.

![09](images/09.png)

<iframe src="//player.vimeo.com/video/110926839?title=0&amp;byline=0&amp;portrait=0" width="575" height="323" frameborder="0" webkitallowfullscreen mozallowfullscreen allowfullscreen></iframe>

### From Point Clouds to Meshes

Now that the points were geolocated, they can be load on the PostGIS database and request together (using ```getPointsForID``` ) to get just the ones that intersect with the Flatiron Building polygon on OSM.

The next step, was to make a mesh of it. A mesh is “solid” 3D bbject of it. For that I use a Poisson Reconstructive Algorithm more precisely the [CGAL’s implementation](http://doc.cgal.org/latest/Surface_reconstruction_points_3/) on [LIDar-Tools](https://github.com/tangrams/LIDar-tools/tree/master/xyz2mesh) you will find a program call [```xyz2Mesh```](https://github.com/tangrams/LIDar-tools/tree/master/xyz2mesh) which run this algorithm to reconstruct a mesh out of the point clouds. Alternatively you can use [MeshLab’s](http://meshlab.sourceforge.net/) implementation. 

![11](images/11.gif)

### Meshes to GeoJSON Polygons

Although I love how meshes looks they have to much information for what OSM can hold. This last step on the process is about downsampling the information we collect to something that could be uploaded and shared in the web. 

OSM database doesn’t know to much about 3D Meshes. Basically all buildings are polygons with altitudes. Like shaped cakes this polygons sits once on top of the other. So in order to make our mesh into something OSM can read and other people use have to be sliced and simplified. In [this repository](https://github.com/tangrams/Mesh2OSMSlicer) you will find a program that does exactly that. It slice the mesh inch by inch like a cheese, checking every slice with the previous one, if it finds a significant changes on the area size It will add a new floor with that side on top of our cake. 

![13](images/13.gif)

## Future and limitations

For this process I had use an extraordinary convenient sample. The Flatiron is perfectly isolated. To apply this process to all of the buildings in a city lots of factors have to be taken in care. This had being an exciting experiment that proof that Lidar data (usually from corporates and gob institutions) together with Structure from motion (crowd founded by enthusiast) can be use to construct and improve open data.