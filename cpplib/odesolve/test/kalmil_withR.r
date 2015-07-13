################################################################################
# Solution of the Kalinin-Miljukov (Nash) cascade of linear reservoirs
# by means of a standard ODE solver
################################################################################

#-------------------------------------------------------------------------------
# Initial actions
#-------------------------------------------------------------------------------

rm(list=ls())
require("odesolve")

#-------------------------------------------------------------------------------
# Parameters
#-------------------------------------------------------------------------------

# Number of linked linear reservoirs
	numreserv= 10
# Retention constant of a single reservoir
	retconst=  1000
# Time step
  dt= 900
# Inflow hydrograph
  file_inflow= "/home/dkneis/cpp/dklib/odesolve/test_kalinin-miljukov/q_in.txt"
  q_in= read.table(file_inflow, header=T, sep="\t")
  q_in= q_in[,"q_in"]
# Initial flow rate
  q_init= q_in[1]

# Parameters of the numerical ODE solver
  RTOL= 1.0e-06
  ATOL= 1.0e-03

#-------------------------------------------------------------------------------
# Functions
#-------------------------------------------------------------------------------

# Derivative of a single reservoirs storage volume with respect to time
#   t:    Time (not used in the calculation of the derivative here)
#   y:    Initial values of the states (Storage volumes of the reservoirs)
#   p:    Vector of parameters (here: upstream inflow, retention constant)
deriv= function (t, y, p) {
  q_in= c(p["q_in"])
  k= c(p["retconst"])
  # First reservoir
  #       <In>   <-Out->
  tmp_1=  q_in - 1/k * y[1]
  # Further reservoirs
  n= length(y)
  if (n > 1) {
  #          <------In------>   <----Out--->
	  tmp_2_n= 1/k * y[1:(n-1)] - 1/k * y[2:n]
  }
  # Result
  return(list(dydt= c(tmp_1, tmp_2_n)))
}

# Get the steady-state storage volume of a single reservoir given the flow rate.
vol_steadystate= function (k, q) {
  return( q * k) 
}

#-------------------------------------------------------------------------------
# Main
#-------------------------------------------------------------------------------

# Allocate matrix to hold the result states (storage volumes for all times)
# Rows = Time steps
# Cols = Reservoirs (time is stored in column 1).
m= matrix(NA, nrow= 1+length(q_in), ncol= 1+numreserv)

# Initialize the states
v= rep(vol_steadystate(retconst, q_init), numreserv)

# Compute the states for all time steps
for (itime in 1:length(q_in)) {
  print(paste("Time step:",itime))
	res= lsoda(y= v,
          times= c((itime-1)*dt, itime*dt),
          func= deriv,
          parms= c(q_in= q_in[itime], retconst= retconst),
          rtol= RTOL,
          atol= ATOL,
          tcrit=NULL, jacfunc=NULL, verbose=FALSE, dllname=NULL, hmin=0, hmax=Inf)
  # Save state
  if (itime == 1) {
    m[1:2,]= res[1:2,]
  } else {
    m[itime+1,]= res[2,]
  }
  # Update state vector
  v= res[2,2:ncol(m)]
}

# Show states
plot(x=range(m[,1]), y=range(m[,2:ncol(m)]), type="n", xlab="Time (s)",
  ylab="V", main="Solution in R")
clr= rainbow(ncol(m)-1)
for (i in 2:ncol(m)) {
  lines(x=m[,1], y=m[,i], col=clr[i-1])
}
for (i in 2:ncol(m)) {
  text(x=0.7*max(m[,1]), y=(ncol(m)-i+1)/ncol(m)*max(m[,2:ncol(m)]), cex=0.5,
    paste("res",(i-1)," max=",max(m[,i])," at t=",m[which.max(m[,i]),1],sep=""))
}



