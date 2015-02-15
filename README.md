
## Usage

### Build

```bash
make -C src asm2graphs
```

### Test

```bash
make -C tests check
```

### Generate JSON for one ASM file

```bash
$MS_ASM_HOME/scripts/generate.sh $MS_ASM_HOME/tests/test.asm
```

This command will create the directory *test* and the archive *test.tar.gz* in the current directory.
The directory *test* will contain one Call Graph stored in *test.json*.
For each procedure, a file *test-_proc\_label_.json* is created, it contaions the Control Flow Graph (CFG) of the corresponding procedure.
*proc\_label* is either starting by *sub\_* if identified by IDA-PRO, or *rtn\_* if identfied by our analysis.
Call Graph (*test.json*):
```json
{
  "routines": [
    {
      "label":"rtn_0",
      "callees":[
        {
          "label"    :"sub_0"
        },{
          "label"    :"sub_1"
        }
      ]
    },{
      "label":"rtn_1",
      "callees":[
        {
          "label"    :"sub_2"
        }
      ]
    },{
      "label":"rtn_2",
      "callees":[
        
      ]
    }
  ]
}
```
Control Flow Graph for *sub\_0* (*test-sub\_0.json*): 
```json

```


### Batch processing

Given a file ("malwares.lst") which contains a list of malware with absolute path:
```bash
for malware in `cat malwares.lst`
do
  $MS_ASM_HOME/scripts/generate.sh $malware
  rm -rf ${malware%.*} # to save space
done
tar czf malwares.tar.gz *.tar.gz 
```

