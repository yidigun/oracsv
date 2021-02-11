# oracsv
Export oracle database to text file.


## Intro


## Install


## Usage

> <code>__oracsv__ [ __-vhq__ ] [ __-D__ ___name___=___value___ ... ] [ ___task-config-file___ ]</code>

Command line options

* __```-v```__ : print version and exit.
* __```-h```__ : print help text and exit.
* __```-q```__ : suppress log output.
* __```-D```__ : specify task configuration value.
* ___task-config-file___ : config file name. (```-``` means stdin)

```oracvs``` uses three files.

* Task configuration file
* Output file (stdout)
* Log file (stderr)

Here is a most common example.

```
$ cat task-name.yaml
datasource: scott/tiger@mydbserver
query: select * from my_table

$ oracsv task-name.yaml > task-name.csv
```

Task configuration can be specified by ```-D``` parameters. At least ```datasource``` and ```query``` values must be specified.

```
$ oracsv -D datasource=scott/tiger@mydbserver \
         -D query="select * from my_table" \
    > my_table.csv
```

Specify output file manually.

```
$ oracsv -D output.file=task-name.csv task-name.yaml
```

You can save log to file.

```
$ oracsv task-name.yaml > task-name.csv 2> task-name.log

$ oracsv -D output.log=task-name.log task-name.yaml > task-name.csv
```

Command line parameters override task definitions.

```
$ cat task-name.yaml
datasource: scott/tiger@mydbserver
query: |
    SELECT *
      FROM DATATABLE
     WHERE REGDT >= TO_DATE(:start_dt, 'YYYY-MM-DD')
       AND REGDT < TO_DATE(:end_dt, 'YYYY-MM-DD')
params:
    start_dt: 1970-01-01
    end_dt: 9999-12-31

$ orascv -D params.start_dt=2021-01-01 -D params.end_dt=2021-02-01 task-name.yaml \
    > task-name.csv
```

You can combine with [```gzip(1)```](https://linux.die.net/man/1/gzip).

```
$ oracsv task-name.yaml 2> task-name.log | gzip > task-name.csv.gz
```

You can combine with [```split(1)```](https://linux.die.net/man/1/split).

```
$ oracsv -D output.header=false task-name.yaml 2> task-name.log | \
    split -d -l 10000 --additional-suffix=.csv - task-name-
```

### On Unix/Linux

Run as background process.

```
$ nohup oracsv task-name.yaml > task-name.csv 2> task-name.log &
```

Run with "Instant task definition" using [here document](https://en.wikipedia.org/wiki/Here_document).

```
$ oracsv - <<EOF > output.csv
>datasource: scott/tiger@mydbserver
>query: select * from my_table where rownum <= 1000
>EOF
```

### On Windows

```
> oracsv.exe task-name.yaml > task-name.csv 2> task-name.log
```


## Task configuration

### Data Source

Use TNS name.

```
datasource: scott/tiger@mydbserver
```

Or 

```
datasource:
    connection: scott/tiger@mydbserver
    fetchsize: 100
```

Or use custom connection string.

```
datasource:
    username: scott
    password: tiger
    connection: |
        (DESCRIPTION =
            (ADDRESS =
                (PROTOCOL = TCP)
                (HOST = mydbserver.example.com)
                (PORT = 1521)
            )
            (CONNECT_DATA =
                (SERVER = DEDICATED)
                (SERVICE_NAME = ORCLPDB)
            )
        )
    charset: KO16MSWIN949
```

#### Data Source options

* __```datasource.fetchsize```__ : fetch array size. (default: 500)
* __```datasource.charset```__ : session character set. See [Recommended Database Character Sets (Oracle 19c documentation)](https://docs.oracle.com/en/database/oracle/oracle-database/19/nlspg/appendix-A-locale-data.html#GUID-9529D1B5-7366-4195-94B5-0F90F3B472E1) (default: AL32UTF8)

### Query

```
query: select * from test_data
```

or

```
query: |
    select *
      from test_data
     where reg_dt >= trunc(sysdate)
```

#### Query variables and parameters

Define query variable in task config file.

```
query: select * from test_data where reg_dt like :month || '%'
params:
    month: 2021-01
```

And it can override when runtime. This can be useful when you write script or cron job.

```
oracsv -D params.month=`date +%Y-%m` task-name.yaml
```

### Output configurations

* __```output.file```__ : output file. (default: Standard output)
* __```output.log```__ : log file. (default: Standard error)
* __```output.header```__ : ```true``` or ```false```. wheather print header line or not. (default: true)
* __```output.quote```__ : ```auto```, ```always``` or ```never```. wheather quote using double quote character or not. (default: auto)
    - __```auto```__: quote when data contains column or row separator. This option can be slow a little because it do string matching for every data. So, if you are handling large data, consider re-define column/row separator and set this param to ```never```.
    - __```always```__: always quote.
    - __```never```__: never quote.
* __```output.colsep```__ : column separator. (default: ```,```)
* __```output.rowsep```__ : row separator. (default: ```\r\n```)
* __```output.dateformat```__ : text format string for ```DATE``` or ```TIMESTAMP``` type data. See [```strftime(3)```](https://linux.die.net/man/3/strftime) for details.  (default: %Y-%m-%d %H:%M:%S)


### Full configuration example

```
datasource:
    username: scott
    password: tiger
    connection: |
        (DESCRIPTION =
            (ADDRESS =
                (PROTOCOL = TCP)
                (HOST = mydbserver.example.com)
                (PORT = 1521)
            )
            (CONNECT_DATA =
                (SERVER = DEDICATED)
                (SERVICE_NAME = ORCLPDB)
            )
        )
    fetchsize: 500
    charset: AL32UTF8

query: |
    SELECT *
      FROM DATATABLE
     WHERE REGDT >= TO_DATE(:start_dt, 'YYYY-MM-DD')
       AND REGDT < TO_DATE(:end_dt, 'YYYY-MM-DD')

params:
    start_dt: 2021-01-01
    end_dt: 2021-02-01

output:
    file: __stdout__
    log: __stderr__
    header: true
    quote: auto
    colsep: ","
    rowsep: "\r\n"
    dateformat: %Y-%m-%d %H:%M:%S
```
