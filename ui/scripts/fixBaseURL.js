import * as process from "process";
import * as fs from "fs";
import * as path from "path";

const REPLACE_TOKEN = "/REPLACEASSETURL";
const PATH_TO_GELLY_HTML = "html/gelly/index.html";
const rootDir = path.resolve(process.cwd(), "..");

const htmlContents = fs.readFileSync(
	path.join(rootDir, PATH_TO_GELLY_HTML),
	"utf8",
);

fs.writeFileSync(
	path.join(rootDir, PATH_TO_GELLY_HTML),
	htmlContents.replaceAll(REPLACE_TOKEN, "asset://garrysmod/html/gelly/"),
);
