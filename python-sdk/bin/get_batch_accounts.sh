#!#!/bin/bash

output_path="accounts/"

while getopts :n: OPTION
do
    case $OPTION in
        n)
            index=0
            while true
            do
                if [ $index -ge $OPTARG ]; then
                    break
                fi
                ./get_account.sh
                index=`expr $index + 1`
            done
            
            pub_files=$(ls $output_path | grep "**.public.pem$")
            for file in ${pub_files}
            do 
                rm $output_path$file
            done

            acc_files=$(ls $output_path | grep "**.pem$")
            index=0
            for file in ${acc_files}
            do 
                filename="node_${index}.pem"
                mv $output_path$file $output_path$filename
                index=`expr $index + 1`
            done
            ;;
        ?) 
            echo "error!!!"
            ;;
    esac
done