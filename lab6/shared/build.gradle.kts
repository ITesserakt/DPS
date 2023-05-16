fun DependencyHandlerScope.corba(dependencyNotation: String) =
    add("corba", dependencyNotation)


configurations {
    create("corba")
}

dependencies {
    corba("org.jacorb:jacorb-idl-compiler:3.9")
    corba("org.jacorb:jacorb-omgapi:3.9")

    implementation("info.picocli:picocli:4.7.3")
    implementation("org.jetbrains:annotations:24.0.0")
    implementation("commons-io:commons-io:2.11.0")
}

task<JavaExec>("buildCorba") {
    inputs.dir("proto")
    outputs.dir("build/generated/jacobIDL")

    mainClass.set("org.jacorb.idl.parser")
    classpath = configurations.getByName("corba")

    args = arrayListOf(
        "-d", "build/generated/jacobIDL",
        "-all", "-forceOverwrite",
        *inputs.files.filter { it.extension == "idl" }.map { it.path }.toTypedArray()
    )
}

tasks.withType<JavaCompile> {
    sourceSets.main {
        java.srcDir("${buildDir}/generated/jacobIDL")
    }
}

tasks.compileJava {
    dependsOn(tasks.getByName("buildCorba"))
}