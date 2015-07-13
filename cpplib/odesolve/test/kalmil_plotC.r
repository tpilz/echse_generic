
ifile= "/home/dkneis/cpp/dklib/odesolve/test_kalinin-miljukov/q_out.txt"
dat= read.table(file=ifile, header=T);


plot(x=range(dat[,1]), y=range(dat[,2:ncol(dat)]), type="n", xlab="Time (s)",
  ylab="V", main="Solution in C++")
clr= rainbow(ncol(dat)-1)
for (i in 2:ncol(dat)) {
  lines(x=dat[,1], y=dat[,i], col=clr[i-1])
}
for (i in 2:ncol(dat)) {
  text(x=0.7*max(dat[,1]), y=(ncol(dat)-i+1)/ncol(dat)*max(dat[,2:ncol(dat)]), cex=0.5,
    paste("res",(i-1)," max=",max(dat[,i])," at t=",dat[which.max(dat[,i]),1],sep=""))
}

