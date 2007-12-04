aff = OpenRead["/Users/avp/Desktop/xx/data/bb_cfg1098.aff",
               BinaryFormat -> True];
(*
aff = OpenRead["/Users/avp/Desktop/xx/data/out.aff",
               BinaryFormat -> True];
 *)

readHeader[f_InputStream] :=
  Module[{o,s,c,m},
	 o = BinaryRead[f,"UnsignedInteger64", ByteOrdering->1];
	 s = BinaryRead[f,"UnsignedInteger64", ByteOrdering->1];
	 c = BinaryRead[f,"UnsignedInteger64", ByteOrdering->1];
	 m = BinaryRead[f,"UnsignedInteger128", ByteOrdering->1];
	 {o,s,c}];

idString = BinaryRead[aff, "TerminatedString"];
dSize = BinaryRead[aff, "Byte"];
dBase = BinaryRead[aff, "Byte"];
dMant = BinaryRead[aff, "Byte"];
dMaxexp = BinaryRead[aff, "UnsignedInteger16", ByteOrdering->1];
dMinexp = BinaryRead[aff, "UnsignedInteger16", ByteOrdering->1];
hSize = BinaryRead[aff, "UnsignedInteger32", ByteOrdering->1];
{dtOff, dtSize, dtCount} = readHeader[aff];
{stOff, stSize, stCount} = readHeader[aff];
{tbOff, tbSize, tbCount} = readHeader[aff];

Print[StringJoin["data: offset ", ToString[dtOff],
		 ", size ", ToString[dtSize],
		 ", count ", ToString[dtCount]]];
Print[StringJoin["stable: offset ", ToString[stOff],
		 ", size ", ToString[stSize],
		 ", count ", ToString[stCount]]];
Print[StringJoin["tree: offset ", ToString[tbOff],
		 ", size ", ToString[tbSize],
		 ", count ", ToString[tbCount]]];

(* read symbol table *)
affSym = Table[False, {stCount}];
SetStreamPosition[aff, stOff];
For[i=1, i <= stCount, i++, affSym[[i]]=BinaryRead[aff,"TerminatedString"]];

(* read nodes *)
nodeTypes = {"void", "char", "int", "double", "complex"};
affRoot = {};
affNodes    = Table[False, {tbCount}];
affName     = Table[False, {tbCount}];
affOffset   = Table[False, {tbCount}];
affSize     = Table[False, {tbCount}];
affType     = Table[False, {tbCount}];
affChildren = Table[{}, {tbCount}];

Print["Stable loaded"];

readNode[f_InputStream, i_] :=
  Module[{t,p,n,s,o},
	 t = nodeTypes[[BinaryRead[f,"Byte"]]];
         p = BinaryRead[f, "UnsignedInteger64", ByteOrdering -> 1];
         n = affSym[[BinaryRead[f, "UnsignedInteger32", ByteOrdering -> 1]+1]];
         If [t == "void", s = 0; o = 0,
	       s = BinaryRead[f, "UnsignedInteger32", ByteOrdering->1];
	       o = BinaryRead[f, "UnsignedInteger64", ByteOrdering->1]];
	 affName[[i]] = n;
	 affOffset[[i]] = o;
	 affSize[[i]] = s;
	 affType[[i]] = t;
	 If [p == 0, affRoot = Append[affRoot,i],
               affChildren[[p]] = Append[affChildren[[p]], i]];
	 True]

SetStreamPosition[aff,tbOff];
For[i = 1, i <= tbCount, i++, readNode[aff,i]];

Print["Tree loaded"];

buildNode[i_] :=
  affNodes[[i]] = {name   -> affName[[i]],
		   type   -> affType[[i]],
		   offset -> affOffset[[i]],
		   size   -> affSize[[i]],
                   children -> Map[Function[c, affName[[c]]->affNodes[[c]]],
				   affChildren[[i]]]};

For[i = tbCount, i>= 1, i--, buildNode[i]];

root = Map[Function[c,affName[[c]]->affNodes[[c]]], affRoot];

Print["Tree built"];

resolveKey[path_String] :=
  Module[{ks, d, ch},
	 ks = StringSplit[path, "/"];
         If [ks == {}, {"void", 0, 0},
                d = First[ks] /. root;
                If [d == First[ks], Return[$Failed]];
		For [ks = Rest[ks],
			ks != {},
			ch = children /. d;
			If [ch == children, Return[$Failed]];
			d = First[ks] /. (children /. d);
			ks = Rest[ks]];
		If [ListQ[d], Return[{type /. d, offset /. d, size /. d}]];
		$Failed]];

getData[path_String] :=
  Module[{t,o,s,d,fmt},
	 {t,o,s} = resolveKey[path];
	 fmt = Switch[t,
		      "void", Return[{t,0,{}}],
		      "char", "Character8",
		      "int", "Integer32",
		      "double", "Real64",
		      "complex", "Complex128",
		      _, Return[$Failed]];
	 SetStreamPosition[aff,o];
	 d = BinaryReadList[aff,fmt,s, ByteOrdering->1];
	 {t,s,d}];

	 
