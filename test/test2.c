#include <stdio.h>
int main(int argc, char ** argv)
{
  printf("Content-Type: text/html\n\n");
  printf("<html>\n<head>\n<title>How Do I look?</title>\n");
  printf("</head>\n<body>\n");
  if(argc != 1)
    {
      if(argv[1][0] == 'l')
        {
          if(argv[1][1] == 'o')
            {
              if(argv[1][2] == 'o')
                {
                  if(argv[1][3] == 'k')
                    {
                      if(argv[1][5] == 'y')
                        {
                          if(argv[1][6] == 'u')
                            {
                              if(argv[1][7] == 'm')
                                {
                                  if(argv[1][8] == 'm')
                                    {
                                      if(argv[1][9] == 'y')
                                      {
                                        if(argv[1][10] == '\0')
                                          {
                                            printf("<p><B>Right Answer there my friend</B></p>\n");
                                          }
                                        else
                                          {
                                            printf("<p>Wrong Answer :( YOU DON'T BELONG HERE</p>\n");
                                          }
                                      }
                                    else
                                      {
                                        printf("<p>Wrong Answer :( YOU DON'T BELONG HERE</p>\n");
                                      }
                                    }
                                  else
                                    {
                                      printf("<p>Wrong Answer :( YOU DON'T BELONG HERE</p>\n");
                                    }
                                }
                              else
                                {
                                  printf("<p>Wrong Answer :( YOU DON'T BELONG HERE</p>\n");
                                }
                            }
                          else
                            {
                              printf("<p>Wrong Answer :( YOU DON'T BELONG HERE</p>\n");
                            }
                        }
                      else
                        {
                          printf("<p>Wrong Answer :( YOU DON'T BELONG HERE</p>\n");
                        }
                    }
                  else
                    {
                      printf("<p>We are here to talk about my look</p>\n");
                    }
                }
              else
                {
                  printf("<p>We are here to talk about my look</p>\n");
                }
            }
          else
            {
              printf("<p>We are here to talk about my look</p>\n");
            }
        }
      else
        {
          printf("<p>We are here to talk about my look</p>\n");
        }
    }
  else
    {
      printf("<p><strong>There is no input in query:( </strong></p>\n");
    }
    printf("</body>\n</html>\n");
}
