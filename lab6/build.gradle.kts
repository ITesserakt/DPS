fun DependencyHandlerScope.corba(dependencyNotation: String) =
    add("corba", dependencyNotation)

plugins {
    id("java")
}

group = "org.tesserakt"
version = "1.0-SNAPSHOT"

configurations {
    create("corba")
}

repositories {
    mavenCentral()
}

dependencies {
    testImplementation(platform("org.junit:junit-bom:5.9.1"))
    testImplementation("org.junit.jupiter:junit-jupiter")
    corba("org.jacorb:jacorb-idl-compiler:3.6.1")
    corba("org.jacorb:jacorb-omgapi:3.6.1")
}

tasks.test {
    useJUnitPlatform()
}

task<JavaExec>("buildCorba") {
    outputs.dir("build/generated/jacobIDL")

    mainClass.set("org.jacorb.idl.parser")
    classpath = configurations.getByName("corba")
    args = arrayListOf(
        "-d", "build/generated/jacorbIDL",
        "-all", "-forceOverwrite"
    )
}