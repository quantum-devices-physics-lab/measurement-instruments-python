#This is an example of the use of the api for the one tone experiment
#to do a simple double measurement with low and high power
#so the we can see the punch out effect in the qubit
#you can either execute this script or run one  command below at a time in the python console


import One_Tone as ot

 #loads the one tone class into exp
 #this will print out the default setup
exp = ot.One_tone()

#using the names on the print out of the default setup one can change it, paying attention to the formats
#the method print_setup() will show these values too
exp.freq_range([10,50])
exp.ampl(8)

#loads the experiment graph screen
#loading another will create another window but the graphs only plot on the last window
exp.graph_window()


#this runs the experiment and if a graph window is existent graphs into it in real time 
exp.run()


#now we increase the amp and thus the power to obtain the punched out graph with the qubit on the excited state
exp.ampl(11)

#and we run it again
#this will plot the experiment in the same window 
#to plot it in other window run the method graph_window()  
exp.run()

input("Enter anything to close\n")
