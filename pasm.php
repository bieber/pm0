#!/usr/bin/php5 -q 

<?php

    /*
    WARNING!  This script does effectively zero error checking, and it will do
    unexpected and horrifying things if you botch it's input (or if I botched
    its implementation)
    */

    if(argc < 3)
        $fout = fopen($argv[2], "w");
    else
        $fout = fopen("php://stdout", "w");
    
    $code = file_get_contents($argv[1]);
    
    $code = explode("\n", $code);
    
    //First pass is gathering labels
    $labels = 0;
    $pc = 0;
    $labels = array();
    foreach($code as $line){
        $line = trim($line);
        $end = substr($line, strlen($line) - 1, 1);
        if($end == ":")
            $labels[substr($line, 0, strlen($line) -1)] = $pc;
        else
            $pc++;
    }
    
    //Second pass is generating output
    foreach($code as $line){
        $line = trim($line);
        $end = substr($line, strlen($line) - 1, 1);
        if($end == ":")
            continue;
        
        //If this line isn't a label, generate an opcode for it
        $line = explode(" ", trim($line));
        $line[0] = strtolower($line[0]);
        switch($line[0]){
        
        case 'lit':
            writeCode($fout, 1, 0, $line[1]);
            break;
            
        case 'ret':
            writeCode($fout, 2, 0, 0);
            break;
            
        case 'neg':
            writeCode($fout, 2, 0, 1);
            break;
            
        case 'add':
            writeCode($fout, 2, 0, 2);
            break;
            
        case 'sub':
            writeCode($fout, 2, 0, 3);
            break;
            
        case 'mul':
            writeCode($fout, 2, 0, 4);
            break;
            
        case 'div':
            writeCode($fout, 2, 0, 5);
            break;
            
        case 'odd':
            writeCode($fout, 2, 0, 6);
            break;
            
        case 'mod':
            writeCode($fout, 2, 0, 7);
            break;
            
        case 'eql':
            writeCode($fout, 2, 0, 8);
            break;
            
        case 'neq':
            writeCode($fout, 2, 0, 9);
            break;
            
        case 'lss':
            writeCode($fout, 2, 0, 10);
            break;
            
        case 'leq':
            writeCode($fout, 2, 0, 11);
            break;
            
        case 'gtr':
            writeCode($fout, 2, 0, 12);
            break;
            
        case 'geq':
            writeCode($fout, 2, 0, 13);
            break;
            
        case 'lod':
            writeCode($fout, 3, $line[1], $line[2]);
            break;
            
        case 'sto':
            writeCode($fout, 4, $line[1], $line[2]);
            break;
            
        case 'cal':
            writeCode($fout, 5, $line[1], $line[2]);
            break;
            
        case 'inc':
            writeCode($fout, 6, 0, $line[1]);
            break;
            
        case 'jmp':
            writeCode($fout, 7, 0, $labels[trim($line[1])]);
            break;
            
        case 'jpc':
            writeCode($fout, 8, 0, $labels[trim($line[1])]);
            break;
            
        case 'soi':
            writeCode($fout, 9, 0, 1);
            break;
            
        case 'sio':
            writeCode($fout, 10, 0, 2);
            break;
        
        case '':
            break;
            
        default:
            fprintf($fout, "YOUR ASSEMBLY IS BORKED\n");
        }
        
    }
    
    function writeCode($fout, $x, $y, $z){
        fprintf($fout, "%d %d %d\n", $x, $y, $z);
    }
    
?>
