#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>


static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}



int main(int argc, char* argv[])
{
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;
   char *str;


void set(char *a,int b, char *c)
{
char *x = a;
 int y = b;
 char *z = c;



 /* Open database */
   rc = sqlite3_open("project.db", &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      fprintf(stderr, "Opened database successfully\n");
   }

 /* Create SQL statement */
   sql = "CREATE TABLE VISITOR("  \
         "FILEPATH TEXT     NOT NULL," \
         "ID      TEXT     NOT NULL,"\
         "NAME  TEXT PRIMARY KEY    NOT NULL," \
         "TIME         timestamp not null default (datetime('now','localtime')) );";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
   fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Table created successfully\n");
   }

str = sqlite3_mprintf("INSERT OR REPLACE INTO VISITOR(FILEPATH,ID,NAME) VALUES ( '%s', %d, '%s' )",x, y, z);
   /* Execute SQL statement */
   rc = sqlite3_exec(db, str, callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Records created successfully\n");
   }


}
 set( "www.baidu.con", 4, "axis");
   sqlite3_close(db);
   return 0;
}

