
#include "patch_parse.h"

	bool quoted = git_parse_ctx_contains_s(&ctx->parse_ctx, "\"");
	for (len = quoted; len < ctx->parse_ctx.line_len; len++) {
		if (!quoted && git__isspace(ctx->parse_ctx.line[len]))
		else if (quoted && !inquote && ctx->parse_ctx.line[len] == '"') {
		inquote = (!inquote && ctx->parse_ctx.line[len] == '\\');
static int parse_header_path_buf(git_buf *path, git_patch_parse_ctx *ctx, size_t path_len)
	int error;
	if ((error = git_buf_put(path, ctx->parse_ctx.line, path_len)) < 0)
	git_parse_advance_chars(&ctx->parse_ctx, path_len);
	int error = parse_header_path_buf(&path, ctx, header_path_len(ctx));
	git_buf old_path = GIT_BUF_INIT;
	int error;

	if ((error = parse_header_path_buf(&old_path, ctx, ctx->parse_ctx.line_len - 1)) <  0)
		goto out;

	patch->old_path = git_buf_detach(&old_path);

out:
	git_buf_free(&old_path);
	return error;
	git_buf new_path = GIT_BUF_INIT;
	int error;

	if ((error = parse_header_path_buf(&new_path, ctx, ctx->parse_ctx.line_len - 1)) <  0)
		goto out;

	patch->new_path = git_buf_detach(&new_path);

out:
	git_buf_free(&new_path);
	return error;
	int64_t m;
	if ((git_parse_advance_digit(&m, &ctx->parse_ctx, 8)) < 0)
		return git_parse_err("invalid file mode at line %"PRIuZ, ctx->parse_ctx.line_num);
	return 0;
	for (len = 0; len < ctx->parse_ctx.line_len && len < GIT_OID_HEXSZ; len++) {
		if (!git__isxdigit(ctx->parse_ctx.line[len]))
		git_oid_fromstrn(oid, ctx->parse_ctx.line, len) < 0)
		return git_parse_err("invalid hex formatted object id at line %"PRIuZ,
			ctx->parse_ctx.line_num);
	git_parse_advance_chars(&ctx->parse_ctx, len);
	char c;

		git_parse_advance_expected_str(&ctx->parse_ctx, "..") < 0 ||
	if (git_parse_peek(&c, &ctx->parse_ctx, 0) == 0 && c == ' ') {
		git_parse_advance_chars(&ctx->parse_ctx, 1);
	if (parse_header_path_buf(&path, ctx, header_path_len(ctx)) < 0)
	int64_t val;
	if (git_parse_advance_digit(&val, &ctx->parse_ctx, 10) < 0)
	if (git_parse_advance_expected_str(&ctx->parse_ctx, "%") < 0)
	if (val < 0 || val > 100)
		return git_parse_err("invalid similarity percentage at line %"PRIuZ,
			ctx->parse_ctx.line_num);
		return git_parse_err("invalid similarity percentage at line %"PRIuZ,
			ctx->parse_ctx.line_num);
static int parse_header_start(git_patch_parsed *patch, git_patch_parse_ctx *ctx)
{
	if (parse_header_path(&patch->header_old_path, ctx) < 0)
		return git_parse_err("corrupt old path in git diff header at line %"PRIuZ,
			ctx->parse_ctx.line_num);

	if (git_parse_advance_ws(&ctx->parse_ctx) < 0 ||
		parse_header_path(&patch->header_new_path, ctx) < 0)
		return git_parse_err("corrupt new path in git diff header at line %"PRIuZ,
			ctx->parse_ctx.line_num);

	/*
	 * We cannot expect to be able to always parse paths correctly at this
	 * point. Due to the possibility of unquoted names, whitespaces in
	 * filenames and custom prefixes we have to allow that, though, and just
	 * proceeed here. We then hope for the "---" and "+++" lines to fix that
	 * for us.
	 */
	if (!git_parse_ctx_contains(&ctx->parse_ctx, "\n", 1)) {
		git_parse_advance_chars(&ctx->parse_ctx, ctx->parse_ctx.line_len - 1);

		git__free(patch->header_old_path);
		patch->header_old_path = NULL;
		git__free(patch->header_new_path);
		patch->header_new_path = NULL;
	}

	return 0;
}

typedef enum {
	STATE_START,

	STATE_DIFF,
	STATE_FILEMODE,
	STATE_MODE,
	STATE_INDEX,
	STATE_PATH,

	STATE_SIMILARITY,
	STATE_RENAME,
	STATE_COPY,

	STATE_END,
} parse_header_state;

	parse_header_state expected_state;
	parse_header_state next_state;
} parse_header_transition;

static const parse_header_transition transitions[] = {
	/* Start */
	{ "diff --git "         , STATE_START,      STATE_DIFF,       parse_header_start },

	{ "deleted file mode "  , STATE_DIFF,       STATE_FILEMODE,   parse_header_git_deletedfilemode },
	{ "new file mode "      , STATE_DIFF,       STATE_FILEMODE,   parse_header_git_newfilemode },
	{ "old mode "           , STATE_DIFF,       STATE_MODE,       parse_header_git_oldmode },
	{ "new mode "           , STATE_MODE,       STATE_END,        parse_header_git_newmode },

	{ "index "              , STATE_FILEMODE,   STATE_INDEX,      parse_header_git_index },
	{ "index "              , STATE_DIFF,       STATE_INDEX,      parse_header_git_index },
	{ "index "              , STATE_END,        STATE_INDEX,      parse_header_git_index },

	{ "--- "                , STATE_INDEX,      STATE_PATH,       parse_header_git_oldpath },
	{ "+++ "                , STATE_PATH,       STATE_END,        parse_header_git_newpath },
	{ "GIT binary patch"    , STATE_INDEX,      STATE_END,        NULL },
	{ "Binary files "       , STATE_INDEX,      STATE_END,        NULL },

	{ "similarity index "   , STATE_DIFF,       STATE_SIMILARITY, parse_header_similarity },
	{ "dissimilarity index ", STATE_DIFF,       STATE_SIMILARITY, parse_header_dissimilarity },
	{ "rename from "        , STATE_SIMILARITY, STATE_RENAME,     parse_header_renamefrom },
	{ "rename old "         , STATE_SIMILARITY, STATE_RENAME,     parse_header_renamefrom },
	{ "copy from "          , STATE_SIMILARITY, STATE_COPY,       parse_header_copyfrom },
	{ "rename to "          , STATE_RENAME,     STATE_END,        parse_header_renameto },
	{ "rename new "         , STATE_RENAME,     STATE_END,        parse_header_renameto },
	{ "copy to "            , STATE_COPY,       STATE_END,        parse_header_copyto },

	/* Next patch */
	{ "diff --git "         , STATE_END,        0,                NULL },
	{ "@@ -"                , STATE_END,        0,                NULL },
	{ "-- "                 , STATE_END,        0,                NULL },
	parse_header_state state = STATE_START;
	for (; ctx->parse_ctx.remain_len > 0; git_parse_advance_line(&ctx->parse_ctx)) {
		if (ctx->parse_ctx.line_len == 0 || ctx->parse_ctx.line[ctx->parse_ctx.line_len - 1] != '\n')
		for (i = 0; i < ARRAY_SIZE(transitions); i++) {
			const parse_header_transition *transition = &transitions[i];
			size_t op_len = strlen(transition->str);
			if (transition->expected_state != state ||
			    git__prefixcmp(ctx->parse_ctx.line, transition->str) != 0)
			state = transition->next_state;

			if (transition->fn == NULL)
			git_parse_advance_chars(&ctx->parse_ctx, op_len);
			if ((error = transition->fn(patch, ctx)) < 0)
			git_parse_advance_ws(&ctx->parse_ctx);
			if (git_parse_advance_expected_str(&ctx->parse_ctx, "\n") < 0 ||
			    ctx->parse_ctx.line_len > 0) {
				error = git_parse_err("trailing data at line %"PRIuZ, ctx->parse_ctx.line_num);

			error = git_parse_err("invalid patch header at line %"PRIuZ,
				ctx->parse_ctx.line_num);
	if (state != STATE_END) {
		error = git_parse_err("unexpected header line %"PRIuZ, ctx->parse_ctx.line_num);
		goto done;
	}

	if (!git__isdigit(ctx->parse_ctx.line[0]))
	if (git__strntol64(&num, ctx->parse_ctx.line, ctx->parse_ctx.line_len, &end, 10) < 0)
	git_parse_advance_chars(&ctx->parse_ctx, (end - ctx->parse_ctx.line));
	if (git_parse_advance_digit(&num, &ctx->parse_ctx, 10) < 0 || !git__is_int(num))
	const char *header_start = ctx->parse_ctx.line;
	char c;
	if (git_parse_advance_expected_str(&ctx->parse_ctx, "@@ -") < 0 ||
	if (git_parse_peek(&c, &ctx->parse_ctx, 0) == 0 && c == ',') {
		if (git_parse_advance_expected_str(&ctx->parse_ctx, ",") < 0 ||
	if (git_parse_advance_expected_str(&ctx->parse_ctx, " +") < 0 ||
	if (git_parse_peek(&c, &ctx->parse_ctx, 0) == 0 && c == ',') {
		if (git_parse_advance_expected_str(&ctx->parse_ctx, ",") < 0 ||
	if (git_parse_advance_expected_str(&ctx->parse_ctx, " @@") < 0)
	git_parse_advance_line(&ctx->parse_ctx);
	hunk->hunk.header_len = ctx->parse_ctx.line - header_start;
		return git_parse_err("oversized patch hunk header at line %"PRIuZ,
			ctx->parse_ctx.line_num);
		ctx->parse_ctx.line_num);
		ctx->parse_ctx.remain_len > 1 &&
		!git_parse_ctx_contains_s(&ctx->parse_ctx, "@@ -");
		git_parse_advance_line(&ctx->parse_ctx)) {
		char c;
		if (ctx->parse_ctx.line_len == 0 || ctx->parse_ctx.line[ctx->parse_ctx.line_len - 1] != '\n') {
			error = git_parse_err("invalid patch instruction at line %"PRIuZ,
				ctx->parse_ctx.line_num);
		git_parse_peek(&c, &ctx->parse_ctx, 0);

		switch (c) {
			/* fall through */
			error = git_parse_err("invalid patch hunk at line %"PRIuZ, ctx->parse_ctx.line_num);
		line->content = ctx->parse_ctx.line + prefix;
		line->content_len = ctx->parse_ctx.line_len - prefix;
		line->content_offset = ctx->parse_ctx.content_len - ctx->parse_ctx.remain_len;
		error = git_parse_err(
	if (git_parse_ctx_contains_s(&ctx->parse_ctx, "\\ ") &&
			error = git_parse_err("cannot trim trailing newline of empty line");
		git_parse_advance_line(&ctx->parse_ctx);
	for (; ctx->parse_ctx.remain_len > 0; git_parse_advance_line(&ctx->parse_ctx)) {
		if (ctx->parse_ctx.line_len < 6)
		if (git_parse_ctx_contains_s(&ctx->parse_ctx, "@@ -")) {
			size_t line_num = ctx->parse_ctx.line_num;
			error = git_parse_err("invalid hunk header outside patch at line %"PRIuZ,
		if (ctx->parse_ctx.remain_len < ctx->parse_ctx.line_len + 6)
		if (git_parse_ctx_contains_s(&ctx->parse_ctx, "diff --git ")) {
	if (git_parse_ctx_contains_s(&ctx->parse_ctx, "literal ")) {
		git_parse_advance_chars(&ctx->parse_ctx, 8);
	} else if (git_parse_ctx_contains_s(&ctx->parse_ctx, "delta ")) {
		git_parse_advance_chars(&ctx->parse_ctx, 6);
		error = git_parse_err(
			"unknown binary delta type at line %"PRIuZ, ctx->parse_ctx.line_num);
	if (git_parse_advance_digit(&len, &ctx->parse_ctx, 10) < 0 ||
	    git_parse_advance_nl(&ctx->parse_ctx) < 0 || len < 0) {
		error = git_parse_err("invalid binary size at line %"PRIuZ, ctx->parse_ctx.line_num);
	while (ctx->parse_ctx.line_len) {
		char c;
		git_parse_peek(&c, &ctx->parse_ctx, 0);

			error = git_parse_err("invalid binary length at line %"PRIuZ, ctx->parse_ctx.line_num);
		git_parse_advance_chars(&ctx->parse_ctx, 1);
		if (encoded_len > ctx->parse_ctx.line_len - 1) {
			error = git_parse_err("truncated binary data at line %"PRIuZ, ctx->parse_ctx.line_num);
			&decoded, ctx->parse_ctx.line, encoded_len, decoded_len)) < 0)
			error = git_parse_err("truncated binary data at line %"PRIuZ, ctx->parse_ctx.line_num);
		git_parse_advance_chars(&ctx->parse_ctx, encoded_len);
		if (git_parse_advance_nl(&ctx->parse_ctx) < 0) {
			error = git_parse_err("trailing data at line %"PRIuZ, ctx->parse_ctx.line_num);
	if (git_parse_advance_expected_str(&ctx->parse_ctx, "GIT binary patch") < 0 ||
		git_parse_advance_nl(&ctx->parse_ctx) < 0)
		return git_parse_err("corrupt git binary header at line %"PRIuZ, ctx->parse_ctx.line_num);
	if (git_parse_advance_nl(&ctx->parse_ctx) < 0)
		return git_parse_err("corrupt git binary separator at line %"PRIuZ,
			ctx->parse_ctx.line_num);
	if (git_parse_advance_nl(&ctx->parse_ctx) < 0)
		return git_parse_err("corrupt git binary patch separator at line %"PRIuZ,
			ctx->parse_ctx.line_num);
	if (git_parse_advance_expected_str(&ctx->parse_ctx, "Binary files ") < 0 ||
		git_parse_advance_expected_str(&ctx->parse_ctx, patch->header_old_path) < 0 ||
		git_parse_advance_expected_str(&ctx->parse_ctx, " and ") < 0 ||
		git_parse_advance_expected_str(&ctx->parse_ctx, patch->header_new_path) < 0 ||
		git_parse_advance_expected_str(&ctx->parse_ctx, " differ") < 0 ||
		git_parse_advance_nl(&ctx->parse_ctx) < 0)
		return git_parse_err("corrupt git binary header at line %"PRIuZ, ctx->parse_ctx.line_num);
	while (git_parse_ctx_contains_s(&ctx->parse_ctx, "@@ -")) {
	if (git_parse_ctx_contains_s(&ctx->parse_ctx, "GIT binary patch"))
	else if (git_parse_ctx_contains_s(&ctx->parse_ctx, "Binary files "))
		return git_parse_err("expected %s path of '/dev/null'", old_or_new);
		return git_parse_err("mismatched %s path names", old_or_new);
		return git_parse_err(
		return git_parse_err("missing new path");
		return git_parse_err("missing old path");
		return git_parse_err("git diff header lacks old / new paths");
		return git_parse_err("patch with no hunks");
	if ((git_parse_ctx_init(&ctx->parse_ctx, content, content_len)) < 0) {
		git__free(ctx);
		return NULL;
	git_parse_ctx_clear(&ctx->parse_ctx);
	start = ctx->parse_ctx.remain_len;
	used = start - ctx->parse_ctx.remain_len;
	ctx->parse_ctx.remain += used;
	GIT_REFCOUNT_INC(&patch->base);