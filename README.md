# jff

This project is for running analysis on the NSE data generated everyday and
is available for download through exchange website.

## Pre-requisites:
  1. Subscribe to the NSE RSS feed
    http://feeds.feedburner.com/nseindia/CMDailyReport
    this feed will send daily update of the marked.

  2. From feed download Bhavcopy file which is named as "cm03AUG2018bhav.csv.zip"
    extract the csv file and place them in a folder.

## Environment and Compiling:
  1. gcc 5.4.0 on Ubuntu 16.04 is where this has been tested for compilation.
  2. script mk is provided with compile commands. provide execute permission to
     this file and execute

## Running:
  1. compilation shall generate the following executable files:
    a. jff
    b. jfftech
    c. jffsort
    d. jffreport

### jff:
  this should be executed with directory as parameter, which contains the csv
  files which were downloaded from nse site (pre-requisites - 2.)

  executing this program shall generate another folder named "dat" in the
  location where this was executed from.

  the dat folder shall contain files for individual companies with daily
  data.

### jffsort:
  this should be executed with input parameter containing the path+directory
  name of companies data

  upon execution the data in individual companies file shall be sorted earliest
  to latest.

### jfftech:
  this again takes dat folder as input parameter. this program runs technical
  analysis on individual companies and populates them within the datastructure
  of the file.

### jffreport:
  takes dat folder as input parameter, and generates the report. output is
  again put in files: bulk.op and today.op

the above and more are scripted in a single file named go assuming the initial
csv files are placed in ./tt folder


