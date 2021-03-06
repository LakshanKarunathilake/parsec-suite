
#include "main/glheader.h"

#include "shader/program.h"
#include "tnl/tnl.h"
#include "r300_context.h"
#include "r300_fragprog.h"

static struct gl_program *r300NewProgram(GLcontext * ctx, GLenum target,
					 GLuint id)
{
	r300ContextPtr rmesa = R300_CONTEXT(ctx);
	struct r300_vertex_program_cont *vp;
	struct r300_fragment_program *r300_fp;
	struct r500_fragment_program *r500_fp;

	switch (target) {
	case GL_VERTEX_STATE_PROGRAM_NV:
	case GL_VERTEX_PROGRAM_ARB:
		vp = CALLOC_STRUCT(r300_vertex_program_cont);
		return _mesa_init_vertex_program(ctx, &vp->mesa_program,
						 target, id);
	case GL_FRAGMENT_PROGRAM_ARB:
		if (rmesa->radeon.radeonScreen->chip_family >= CHIP_FAMILY_RV515) {
			r500_fp = CALLOC_STRUCT(r500_fragment_program);
			r500_fp->ctx = ctx;
			return _mesa_init_fragment_program(ctx, &r500_fp->mesa_program,
							   target, id);
		} else {
			r300_fp = CALLOC_STRUCT(r300_fragment_program);
			return _mesa_init_fragment_program(ctx, &r300_fp->mesa_program,
							   target, id);
		}

	case GL_FRAGMENT_PROGRAM_NV:
		if (rmesa->radeon.radeonScreen->chip_family >= CHIP_FAMILY_RV515) {
			r500_fp = CALLOC_STRUCT(r500_fragment_program);
			return _mesa_init_fragment_program(ctx, &r500_fp->mesa_program,
							   target, id);
		} else {
			r300_fp = CALLOC_STRUCT(r300_fragment_program);
			return _mesa_init_fragment_program(ctx, &r300_fp->mesa_program,
							   target, id);
		}
	default:
		_mesa_problem(ctx, "Bad target in r300NewProgram");
	}

	return NULL;
}

static void r300DeleteProgram(GLcontext * ctx, struct gl_program *prog)
{
	_mesa_delete_program(ctx, prog);
}

static void
r300ProgramStringNotify(GLcontext * ctx, GLenum target, struct gl_program *prog)
{
	r300ContextPtr rmesa = R300_CONTEXT(ctx);
	struct r300_vertex_program_cont *vp = (void *)prog;
	struct r300_fragment_program *r300_fp = (struct r300_fragment_program *)prog;
	struct r500_fragment_program *r500_fp = (struct r500_fragment_program *)prog;

	switch (target) {
	case GL_VERTEX_PROGRAM_ARB:
		vp->progs = NULL;
		break;
	case GL_FRAGMENT_PROGRAM_ARB:
		if (rmesa->radeon.radeonScreen->chip_family >= CHIP_FAMILY_RV515)
			r500_fp->translated = GL_FALSE;
		else
			r300_fp->translated = GL_FALSE;
		break;
	}

	/* need this for tcl fallbacks */
	_tnl_program_string(ctx, target, prog);
}

static GLboolean
r300IsProgramNative(GLcontext * ctx, GLenum target, struct gl_program *prog)
{
	return GL_TRUE;
}

void r300InitShaderFuncs(struct dd_function_table *functions)
{
	functions->NewProgram = r300NewProgram;
	functions->DeleteProgram = r300DeleteProgram;
	functions->ProgramStringNotify = r300ProgramStringNotify;
	functions->IsProgramNative = r300IsProgramNative;
}
