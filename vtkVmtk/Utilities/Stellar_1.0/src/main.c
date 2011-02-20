/*****************************************************************************/
/*                                                                           */
/*  main()                                                                   */
/*                                                                           */
/*  This is basically a harness function to use Star to read in an existing  */
/*  tetrahedral mesh, perform the improvement, and then output the improved  */
/*  mesh. It will undoubtedly need to be updated and generalized as Star's   */
/*  mesh generation capabilities mature (e.g., the ability to read in        */
/*  constraining facets or tetrahedralize arbitrary inputs).                 */
/*                                                                           */
/*****************************************************************************/

int main(int argc,
         char **argv)
{
  struct behavior behave;
  struct inputs in;
  struct outputs out;
  struct proxipool vertexpool;
  struct tetcomplex mesh;
  FILE *polyfile;

  primitivesinit();
  
  /* tack on -rNE every time, to enforce standard Star behavior */
  argv[argc] = "-rNE";
  argc += 1;
  
  /* call Star's command line parsing */
  parsecommandline(argc, argv, &behave);
  polyfile = inputverticesreadsortstore(&behave, &in, &vertexpool);

  /* parse improvement options */
  parseimprovecommandline(argc, argv, &improvebehave);

  /* Compute an array mapping vertex numbers to their tags. */
  inputmaketagmap(&vertexpool, in.firstnumber, in.vertextags);
  /* Read and reconstruct a mesh. */
  inputtetrahedra(&behave, &in, &vertexpool, &out, &mesh);

  /*********************************************************/
  /* IMPROVEMENT HAPPENS HERE                              */
  /*********************************************************/
  staticimprove(&behave, &in, &vertexpool, &mesh, argc, argv);

  /* Free the array of vertex tags allocated by inputall(). */
  if (in.vertextags != (tag *) NULL) {
    starfree(in.vertextags);
    in.vertextags = (tag *) NULL;
  }
  /* Free the pool of vertices initialized by inputall(). */
  proxipooldeinit(&vertexpool);

  return 0;
}
