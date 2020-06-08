/**
 * This is the analytics script that should be injected in the page you want to analyze:
 * <script type="text/javascript" src="http://somewhere.com/cgi-bin/inject_script.cgi"></script>
 */
(function() {
    const referer = encodeURIComponent(document.referrer);
    const lang = navigator.language;
    fetch(`//example.com/cgi-bin/record.cgi?referer=${referer}&lang=${lang}`)
        .then(res => res.json())
        .then(data => null)
        .catch(err => null);
})();