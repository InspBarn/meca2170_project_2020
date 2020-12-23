# meca2170_project_2020

This project has been build during a course about numerical geometry. Its purpose is the computation of the convex hull of a set of points according to four different methods : Jarvis March, Graham Scan, Chan and Quick Hull. The project is composed of three different files in addition to the main.c file which is the one to run :

1. inputs.* : it generates a random set of N points whether randomly or under the shape of a polygon. 
2. utils.* : you will find here all the useful functions we added along the project to make the code work correctly. You will also find a new data structure convex_hull_t which takes as argument a set of N points, its convex hull as well as datas structure of the "BOV.h" library to plot these.
3. convex_hull.* : you will find here the four algorithm we implemented. 


In order to make them run, go into the main and choice the algorithm of your choice. 
You may want to display the animation of the hull being constructed but you may do not want it. To make it possible, we initialize one macro variable for each algorithm:

  → JARVIS_ANIMATION for Jarvis March Algorithm 
  
  → GRAHAM_ANIMATION for Graham Scan Algorithm 
  
  → CHAN_ANIMATION for Chan Algorithm, note that to present each subset of points and their convex hull you have CHAN_PRESENTATION_ to set equal to 1 
  
  → QUICK_ALGORITHM for the Quick Hull Algorithm 
  
For displaying, initialize them to 1 in convex-hull.h. But if you don't want to display, set them back to 0.
A last function is used to show the set of points and its final convex hull. The classical commands of the "BOV.h" library are working but you can add or remove points to set by clicking the right button of the mouse. The new convex hull will be automatically recomputed.

Originally the code will show you the chan implementation for a special case. You can change that by putting a 0 at line 18 of main.c in place of the 1 and the choose the algorithm you want
