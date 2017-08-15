#!/bin/bash

ssh -L2003:localhost:2003 ec2-34-249-206-224.eu-west-1.compute.amazonaws.com nc localhost 2003
