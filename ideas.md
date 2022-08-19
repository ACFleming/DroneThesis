# 13/06/22

**Idea**: frontiers for exploration are the same as edges in images. Can i use opencv edge detection to find the centroid of frontiers??

**Idea:**: using the opencv matrix as the occ grid as you can also run maths functions on it

**Decision**

   
    matlab plotting (and my initial assumption)
    y
    ^ 
    |
    |
    |
    |
    * - - - - - > x

    matricies work in row, col referencing.
    * - - - - - > cols
    |
    |
    |
    |
    |
    v
    rows

    open cv points work x,y but top left origin

    * - - - - - > x
    |
    |
    |
    |
    |
    v
    y

    As such, the best representation is to have the origin as top left (like working in the 4th quadrant)



**Note**
An inscribed octagon has 90% of the area of the circle it enscribes. Thinking of using an octagon to get boundary vectors





**Idea** Have been working with contours to combine frontiers. Need to use cv RETR_CCOMP contour detection to find any outside contours of holes, as this will prevent situations where a hole is left during searching




**Note**
Making assumption that the two drones will start at approximately the same location. This saves me from dealing with two distinct frontiers, but it is also a valid assumption because in reality both drones should be launched near each other