#!/usr/bin/env Rscript

data <- read.csv('output/data.csv')
par(bg='black')
par(mar=c(5,5,1,1))
plot(data$cycles, type='l', xlab='Samples', ylab='Cycles', cex.axis=2, cex.lab=2, lwd=4, col='white', col.lab='white', col.main='white', col.axis='white', fg='white')
