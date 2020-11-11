﻿<!DOCTYPE html>
<html>
<head>
  <title>FermiSurfer on Web</title>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/gl-matrix/2.8.1/gl-matrix-min.js"
          integrity="sha512-zhHQR0/H5SEBL3Wn6yYSaTTZej12z0hVZKOv3TwCUXT1z5qeqGcXJLLrbERYRScEDDpYIJhPC1fk31gqR783iQ=="
          crossorigin="anonymous" defer>
  </script>
  <?php if ($_GET['frmsf'] == ""): ?>
    <script type="text/javascript">
      var frmsf = "";
    </script> 
  <?php else: ?>
    <script type="text/javascript" src="<?=$_GET['frmsf']?>"></script>
  <?php endif; ?>
  <script type="text/javascript" src="./fermisurfer.js" defer></script>
</head>
  <body>
      File: <input type="file" id="inputfile" name="inputfile">
    <input type="button" value="read" onclick="read_file();">
    <table border="0">
      <tr align="left">
        <td><canvas id="glcanvas" width="600" height="600" style="border:solid black 1px;"></canvas></td>
        <td>
          <table border="0">
            <tr align="left">
              <td><input type="button" value="update"></td>
              <td>Line width :</td>
              <td><input type="number" name="linewidth" id="linewidth" value="1"></td>
              <td><input type="checkbox" name="ongamma" id="ongamma" value="ongamma" checked="checked">On Gamma</td>
            </tr>
            <tr align="left">
              <td>Section-v : </td>
              <td><input type="number" name="sectionv0" id="sectionv0" value="0"></td>
              <td><input type="number" name="sectionv1" id="sectionv1" value="0"></td>
              <td><input type="number" name="sectionv2" id="sectionv2" value="1"></td>
            </tr>
            <tr align="left">
              <td>Equator-v : </td>
              <td><input type="number" name="equatorv0" id="equatorv0" value="0"></td>
              <td><input type="number" name="equatorv1" id="equatorv1" value="0"></td>
              <td><input type="number" name="equatorv2" id="equatorv2" value="1"></td>
            </tr>
            <tr align="left">
              <td>Interpol ratio : </td>
              <td><input type="number" name="interpol" id="interpol" value="1"></td>
              <td>Fermi energy : </td>
              <td><input type="number" name="fermienergy" id="fermienergy" value="0.0"></td>
            </tr>
            <tr align="left">
              <td>Min. of Scale : </td>
              <td><input type="text" name="scalemin" id="scalemin" value=""></td>
              <td>Max. of Scale : </td>
              <td><input type="text" name="scalemax" id="scalemax" value=""></td>
            </tr>
            <tr align="left">
              <td>Tetrahedron : </td>
              <td></td>
              <td></td>
              <td></td>
            </tr>
            <tr align="left">
              <form id="tetrahedron">
                <td><input type="radio" name="tetrahedron" value="1" checked="checked">1</td>
                <td><input type="radio" name="tetrahedron" value="2">2</td>
                <td><input type="radio" name="tetrahedron" value="3">3</td>
                <td><input type="radio" name="tetrahedron" value="3">4</td>
              </form>
            </tr>
            <tr align="left">
              <td>Color-scale mode : </td>
              <td></td>
              <td></td>
              <td></td>
            </tr>
            <tr align="left">
              <form id="colorscalemode">
                <td><input type="radio" name="colorscalemode" value="1" checked="checked">Input(1D)</td>
                <td><input type="radio" name="colorscalemode" value="2">Input(2D)</td>
                <td><input type="radio" name="colorscalemode" value="3">Input(3D)</td>
                <td><input type="radio" name="colorscalemode" value="4">Fermi velocity</td>
              </form>
            </tr>
            <tr align="left">
              <td>Brillouin zone : </td>
              <form id="brillouinzone">
                <td><input type="radio" name="brillouinzone" value="1" checked="checked">First</td>
                <td><input type="radio" name="brillouinzone" value="-1">Premitive</td>
              </form>
              <td></td>
            </tr>
            <tr align="left">
              <td>Stereogram : </td>
              <form id="stereogram">
                <td><input type="radio" name="stereogram" value="1" checked="checked">None</td>
                <td><input type="radio" name="stereogram" value="2">Parallel</td>
                <td><input type="radio" name="stereogram" value="3">Cross</td>
              </form>
            </tr>
            <tr align="left">
              <td>Mouse drag : </td>
              <form id="mousedrag">
                <td><input type="radio" name="mousedrag" value="1" checked="checked">Rotate</td>
                <td><input type="radio" name="mousedrag" value="2">Scale</td>
                <td><input type="radio" name="mousedrag" value="3">Translate</td>
              </form>
            </tr>
            <tr align="left">
              <td>BZ number : </td>
              <td><input type="number" name="bznumber1" id="bznumber0" value="1"></td>
              <td><input type="number" name="bznumber2" id="bznumber1" value="1"></td>
              <td><input type="number" name="bznumber3" id="bznumber2" value="1"></td>
            </tr>
            <tr align="left">
              <td>Backgraound (RGB) : </td>
              <td><input type="number" name="backgraoundr" id="backgraoundr" value="0"></td>
              <td><input type="number" name="backgraoundg" id="backgraoundg" value="0"></td>
              <td><input type="number" name="backgraoundb" id="backgraoundb" value="0"></td>
            </tr>
            <tr align="left">
              <td>Line color (RGB) : </td>
              <td><input type="number" name="linecolorr" id="linecolorr" value="1"></td>
              <td><input type="number" name="linecolorg" id="linecolorg" value="1"></td>
              <td><input type="number" name="linecolobr" id="linecolorb" value="1"></td>
            </tr>
            <tr align="left">
              <td><input type="button" value="Rotate"></td>
              <td><input type="number" name="rotatex" id="rotatex" value="0"></td>
              <td><input type="number" name="rotatey" id="rotatey" value="0"></td>
              <td><input type="number" name="rotatez" id="rotatez" value="0"></td>
            </tr>
            <tr align="left">
              <td>Scale : <input type="number" name="scale" id="scale" value="1"></td>
              <td>Position : </td>
              <td><input type="number" name="positionx" id="positionx" value="0"></td>
              <td><input type="number" name="positiony" id="positiony" value="0"></td>
            </tr>
            <tr align="left">
              <td><input type="checkbox" name="colorbar" id="colorbar" value="colorbar" checked="checked">Color bar</td>
              <td><input type="checkbox" name="equator" id="equator" value="equator">Equator</td>
              <td><input type="checkbox" name="nodalline" id="nodalline" value="nodalline">Nodal line</td>
              <td><input type="checkbox" name="section" id="section" value="section">Section</td>
            </tr>
            <tr align="left">
              <td>Lighting : </td>
              <form id="lighting">
                <td><input type="radio" name="lighting" value="1.0" checked="checked">Both</td>
                <td><input type="radio" name="lighting" value="-1.0">Occupy</td>
                <td><input type="radio" name="lighting" value="1.0">Unoccupy</td>
              </form>
            </tr>
            <tr align="left">
              <form id="barcolor">
                <td><input type="radio" name="barcolor" value="bgr" checked="checked">BGR</td>
                <td><input type="radio" name="barcolor" value="cmy">CMY</td>
                <td><input type="radio" name="barcolor" value="mcy">MCY</td>
              </form>
            </tr>
            <tr align="left">
              <td><input type="button" value="Section file"></td>
              <td><input type="checkbox" name="band0" id="band0" value="band0" checked="checked">Band 0</td>
              <td><input type="checkbox" name="band1" id="band1" value="band1" checked="checked">Band 1</td>
              <td><input type="checkbox" name="band2" id="band2" value="band2" checked="checked">Band 2</td>
            </tr>
          </table>
        </td>
      </tr>
    </table>

    <pre id="log" style="border: 1px solid #ccc; max-width: 80em; overflow: auto; max-height: 10em;"></pre>
  </body>
</html>
