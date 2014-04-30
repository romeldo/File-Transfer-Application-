File-Transfer-Application-
==========================

******How to test all 5 cases on this FTP Application******

To download enter 'D'
enter  'hello.txt'

To upload enter 'U'
enter  'poo.txt'

To change directory enter 'P'
enter 'dir1', or 'dir2', or 'dir3'  
To test if directory change worked
enter 'U' to upload and enter 'poo.txt'to upload file into new directory.
To go back to /server directory enter 'root'

To test error case:
enter 'D' for download
then enter 'random.txt'
then you should get error "Error: File doesn't exit"
this error message has been sent by server

To list files in a directory
enter 'L' then enter 'dir3' 
you should then get list of all files in directory dir3
